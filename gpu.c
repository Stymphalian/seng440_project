#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>             // memset
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include "mailbox.h"
#include "fft.h"
#include "gpu.h"


#define GPU_MEM_FLG     0xC
#define GPU_MEM_MAP     0x0
#define REGISTER_BASE   0x20C00000

#define NUNIFORMS       3


/*
 * TODO: expand this for multiple QPUs
 */
struct fft_memory_map
{
    /*
       data layout is:
	 twiddles		N/2 complex_t twiddles 
         input (numXsize)	N complex_t samples
   	 output                 N   
   */
    
    complex_t data[1 + (2 + 2) * NUM_QPUS]; //N_2 + N + N
    uint32_t code[MAX_CODE_SIZE];
    /*
	uniforms are:
	    u1: address of twiddle texture 
	    u2: address of input vectors
	    u3: address of output vectors
    */	

    uint32_t uniforms[NUNIFORMS*NUM_QPUS];
    uint32_t msg[NUM_QPUS*2];
};


static struct
{
    int mb;
    unsigned handle;
    unsigned size;
    unsigned vc_msg;
    unsigned ptr;
    void* arm_ptr;
    volatile uint32_t *registers;
} fft_qpu_context;


int fft_setup_qpu(complex_t* input, complex_t* output, complex_t* twiddles, unsigned N,
                   unsigned *shader_code, unsigned code_bytes)
{
    
    fft_qpu_context.mb = mbox_open();
    if (qpu_enable(fft_qpu_context.mb, 1)) {
        fprintf(stderr, "Unable to enable QPU\n");
        return -1;
    }
   
    unsigned N_2 = N /2; //assumes we got N as power of 2
    
    //1MB for now
    fft_qpu_context.size = 1024*1024; //size
    fft_qpu_context.handle = mem_alloc(fft_qpu_context.mb,
                                          fft_qpu_context.size, 4096,
                                          GPU_MEM_FLG);
    if (!fft_qpu_context.handle) {
        fprintf(stderr, "Unable to allocate %d bytes of GPU memory",
                        fft_qpu_context.size);
        return -2;
    }
    unsigned ptr = mem_lock(fft_qpu_context.mb, fft_qpu_context.handle);
    fft_qpu_context.arm_ptr = mapmem(ptr + GPU_MEM_MAP, fft_qpu_context.size);
    fft_qpu_context.ptr = ptr;
    
    printf("Locked memory at 0x%x = 0x%x\n", ptr, fft_qpu_context.arm_ptr);

    struct fft_memory_map *arm_map = (struct fft_memory_map *)
                                                fft_qpu_context.arm_ptr;
    memset(arm_map, 0x0, sizeof(struct fft_memory_map));
    unsigned vc_data = ptr + offsetof(struct fft_memory_map, data);
    unsigned vc_uniforms = ptr + offsetof(struct fft_memory_map, uniforms);
    unsigned vc_code = ptr + offsetof(struct fft_memory_map, code);
    fft_qpu_context.vc_msg = ptr + offsetof(struct fft_memory_map, msg);

    
    printf("Number of twiddles to load: %d\n", N_2);
    printf("Number of output to set asside: %d\n", N);
    memcpy(arm_map->code, shader_code, code_bytes);
    memcpy(arm_map->data, twiddles, N_2*sizeof(complex_t));
    memcpy(arm_map->data+N_2, input, N*sizeof(complex_t)*NUM_QPUS);
    memcpy(arm_map->data+N_2+N*NUM_QPUS, output, N*sizeof(complex_t)*NUM_QPUS);
   
    printf("Setting up the uniforms\n"); 
    int i;
    for (i=0; i < NUM_QPUS; i++) {
        arm_map->uniforms[i*NUNIFORMS+0] = vc_data;         // data (address of twiddle texture)
        arm_map->uniforms[i*NUNIFORMS+1] = vc_data + N_2*sizeof(complex_t) + N * i * sizeof(complex_t);         // address of input vectors
        arm_map->uniforms[i*NUNIFORMS+2] = vc_data + N_2*sizeof(complex_t) + N*NUM_QPUS*sizeof(complex_t) + N * i * sizeof(complex_t);
        
	arm_map->msg[i*2+0] = vc_uniforms + i * NUNIFORMS * sizeof(complex_t);
        arm_map->msg[i*2+1] = vc_code;
    }
    
    //printf("Executing the QPU code ...\n");
    //unsigned ret = execute_qpu(fft_qpu_context.mb, NUM_QPUS, fft_qpu_context.vc_msg, 1, 10000);
    //printf("mailbox: execute_qpu return code: %d\n", ret); 

    return fft_qpu_context.mb;
}


void fft_execute_qpu(unsigned N)
{
    unsigned ret = execute_qpu(fft_qpu_context.mb, NUM_QPUS,
                               fft_qpu_context.vc_msg, 1, 10000);
    if (ret != 0)
        fprintf(stderr, "Failed execute_qpu!\n");
}


void fft_cleanup_qpu(int handle)
{
    unmapmem(fft_qpu_context.arm_ptr, fft_qpu_context.size);
    mem_unlock(fft_qpu_context.mb, fft_qpu_context.handle);
    mem_free(fft_qpu_context.mb, fft_qpu_context.handle);
    qpu_enable(fft_qpu_context.mb, 0);
    mbox_close(fft_qpu_context.mb);
}


void fft_fetch_result(complex_t *output, unsigned N)
{
    unsigned N_2 = N / 2;
    struct fft_memory_map *arm_map = (struct fft_memory_map *)
                                                fft_qpu_context.arm_ptr;
    memcpy(output, arm_map->data+N+N_2, N*NUM_QPUS*sizeof(complex_t));
}


int load_qpu_code(const char *fname, unsigned int* buffer, int len)
{
    FILE *in = fopen(fname, "r");
    if (!in) {
        fprintf(stderr, "Failed to open %s.\n", fname);
        return -1;
    }

    size_t items = fread(buffer, sizeof(unsigned int), len, in);
    fclose(in);

    return items * sizeof(unsigned int);
}
