#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/time.h>

#include "mailbox.h"

#define GPU_MEM_FLG     0xC
#define GPU_MEM_MAP     0x0
#define NUM_QPUS        1
#define MAX_CODE_SIZE   8192

static unsigned int qpu_code[MAX_CODE_SIZE];
#define as_gpu_address(x) (unsigned) gpu_ptr + ((void *)x - arm_ptr)

struct memory_map {
    unsigned int code[MAX_CODE_SIZE];
    unsigned int uniforms[NUM_QPUS][5];    
    unsigned int msg[NUM_QPUS][2];
    unsigned int results[NUM_QPUS][16];     
};

int load_shader_code(const char *fname, unsigned int* buffer, int len)
{
    FILE *in = fopen(fname, "r");
    if (!in) {
        fprintf(stderr, "Failed to open %s.\n", fname);
        exit(0);
    }

    size_t items = fread(buffer, sizeof(unsigned int), len, in);
    fclose(in);

    return items;
}


int main(int argc, char **argv)
{
    unsigned N = 2;
    int i,j;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <code .bin>\n", argv[0]);
        return 0;
    }
    int code_words = load_shader_code(argv[1], qpu_code, MAX_CODE_SIZE);

    printf("Loaded %d bytes of code from %s ...\n", code_words * sizeof(unsigned), argv[1]);

    /* Enable QPU for execution using mailbox&*/
    int mb = mbox_open();
    int qpu_enabled = !qpu_enable(mb, 1);
    if (!qpu_enabled) {
        fprintf(stderr, "QPU enable failed.\n");
        goto cleanup; 
    }
    printf("QPU enabled.\n");


    /*Allocate GPU memory and map it into ARM address space */
    unsigned size = 1024 * 1024; // 1Mb should be enough
    unsigned handle = mem_alloc(mb, size, 4096, GPU_MEM_FLG);
    if (!handle) {
        fprintf(stderr, "Unable to allocate %d bytes of GPU memory", size);
        goto cleanup; 
    }
    unsigned gpu_ptr = mem_lock(mb, handle);
    void *arm_ptr = mapmem(gpu_ptr + GPU_MEM_MAP, size);

    /* Fill the result buffer with 0x55 for debugging */ 
    memset(arm_ptr, 0x55, size);
    struct memory_map *arm_map = (struct memory_map *)arm_ptr;
    memset(arm_map, 0x0, sizeof(struct memory_map));
   
    /* Copy QPU program into GPU memory */ 
    unsigned vc_code = gpu_ptr + offsetof(struct memory_map, code);
    memcpy(arm_map->code, qpu_code, code_words * sizeof(unsigned int));

    /* Get address for msg and results */ 
    unsigned vc_msg = gpu_ptr + offsetof(struct memory_map, msg);
    unsigned vc_results = gpu_ptr + offsetof(struct memory_map, results);
        
    printf("QPU %d msg addr: %08x\n", 0,vc_msg );
    printf("QPU %d result addr: %08x\n", 0,vc_results );
    
    int x1 = 0x1111;
    int x2 = 0x2222;
    int x3 = 0x3333;
    int x4 = 0x4444;

    /* Build Uniforms */
    unsigned vc_uniforms = gpu_ptr + offsetof(struct memory_map, uniforms);
    for (i=0; i < NUM_QPUS; i++) {
        arm_map->uniforms[i][0] = x1;
        arm_map->uniforms[i][1] = x2;
        arm_map->uniforms[i][2] = x3;
        arm_map->uniforms[i][3] = x4;
        arm_map->uniforms[i][4] = vc_results + i * sizeof(unsigned) * 16;
		
        
	/*Build QPU Launch messages*/
	arm_map->msg[i][0] = vc_uniforms + i * sizeof(unsigned) * 2; //GPU Address of uniforms
        arm_map->msg[i][1] = vc_code; //GPU Address of QPU code
    }

	// Test buffer
	printf("before:");
	for (i=0; i<size/4; i++) {
		if ((i%8)==0) printf("\n%08x:", gpu_ptr+i*4);
		printf(" %08x", ((unsigned *)arm_ptr)[i]);
	}
	printf("\n");
    
    unsigned ret = execute_qpu(mb, NUM_QPUS, vc_msg, 1, 10000);

    printf("qpu exec %08x returns %d\n", gpu_ptr + ((void *)vc_msg - arm_ptr), ret);
	// Test buffer
	printf("after:");
	for (i=0; i<size/4; i++) {
		if ((i%8)==0) printf("\n%08x:", gpu_ptr+i*4);
		printf(" %08x", ((unsigned *)arm_ptr)[i]);
	}
	printf("\n");
    
    // check the results!
    for (i=0; i < NUM_QPUS; i++) {
        for (j=0; j < 16; j++) {
            printf("QPU %d, word %d: 0x%08x\n", i, j, arm_map->results[i][j]);
        }
    }

cleanup:
    /* Release GPU memory */
    if (arm_ptr) {
	unmapmem(arm_ptr, size);
    }
    	
    if (handle) {
	mem_unlock(mb, handle);
    	mem_free(mb, handle);
    }

    /* Release QPU */
    if (qpu_enabled) {
    	qpu_enable(mb, 0);
    }
	
    printf("Done.\n");
}
