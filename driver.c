#include "fft.h"
#include "gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>


int main(int argc, char** argv){
	unsigned run_cpu = 1;	

	if( argc < 3){
		printf("Usage: %s [sample_size] [forward|inverse] [-qpu] \n", argv[0]);
		return 0;
	}


	// determine if we should be doing a forward or inverse fft.
	int sample_size = atoi(argv[1]);
	int inverse_flag = atoi(argv[2]);
	int i = 0;
    
	if (argc > 3 && (strcmp(argv[3], "-qpu") == 0)) {
        	printf("Running on the GPU\n");
		run_cpu = 0;
	}
	
	complex_t* input = malloc(sizeof(complex_t)*sample_size);
	complex_t* output = malloc(sizeof(complex_t)*sample_size);
	if(!input || !output){
		fprintf(stderr,"Unable to malloc enough memory\n");
		free(input);
		free(output);
		return 0;
	}

//	printf("Grabbing samples....\n");
	float re,im;
	for(i =0;i < sample_size; ++i){		
		if(EOF == scanf("%f %f",&re,&im) ){
			printf("Not enough samples. Expected %d samples, but only %d read\n",
						sample_size,i);
			return 0;
		}

		// bit len: 32b
		// signed: 1b
		// -2048....2048: 2^15
		// scale factor: 2^16
		#ifdef FIXED_POINT
		input[i].re = scale32i(re,SCALE_FACTOR);
		input[i].im = scale32i(im,SCALE_FACTOR);
		#else
		input[i].re = re;
		input[i].im = im;
		#endif		
	}
    

#ifndef TIME_TEST
	//compute the fft
	if(run_cpu) {
		if(inverse_flag == 1 ){
			inverse_fft(input, output, sample_size);
		}else{
			forward_fft(input, output, sample_size);
		}
	} else {
//		printf("Performing forward fft on gpu\n");
		forward_fft_gpu(input, output, sample_size);
	}	

	for( i = 0; i < sample_size; ++i){		
		// bit len: 32b
		// signed : 1b 
		// scale factor: 16 b
		#ifdef FIXED_POINT
		float re = unscale32i(output[i].re,SCALE_FACTOR);
		float im = unscale32i(output[i].im,SCALE_FACTOR);
		#else
		float re = output[i].re;
		float im = output[i].im;
		#endif

		#ifdef PRINT_OUTPUT
//		printf("After: %f %f\n",re,im);
		printf("%f %f\n",re,im);
		#endif
	}
#else
	//compute the fft
	unsigned total_time = 0;
	unsigned timespent[2];
	int num_runs = 10;
	for(i = 0;i < num_runs ; ++i){
		timespent[0] = Microseconds();
		if(run_cpu) {
			if(inverse_flag == 1 ){
				inverse_fft(input, output, sample_size);
			}else{
				forward_fft(input, output, sample_size);
			}
		} else {
			forward_fft_gpu(input, output, sample_size);
		}	
		
		timespent[1] = Microseconds();
		total_time += (timespent[1] - timespent[0]);
	}
	printf("%10d samples  %10d usecs\n",sample_size,total_time/num_runs);
#endif

	free(input);
	free(output);

	return 0;
}

