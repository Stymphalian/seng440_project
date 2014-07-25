#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

int main(int argc, char** argv){
	if( argc != 3){
		printf("Usage: %s [num_samples] [forward|inverse] \n", argv[0]);
		return 0;
	}

	// determine if we should be doing a forward or inverse fft.
	int number_samples = atoi(argv[1]);
	int inverse_flag = atoi(argv[2]);
	int i = 0;
	
	complex_t* input = malloc(sizeof(complex_t)*number_samples);
	complex_t* output = malloc(sizeof(complex_t)*number_samples);
	if(!input || !output){
		fprintf(stderr,"Unable to malloc enough memory\n");
		free(input);
		free(output);
		return 0;
	}

	float re,im;
	for(i =0;i < number_samples; ++i){		
		if(EOF == scanf("%f %f",&re,&im) ){
			printf("Not enough samples. Expected %d samples, but only %d read\n",
						number_samples,i);
			return 0;
		}

		// bit len: 32b
		// signed: 1b
		// -2048....2048: 2^11
		// scale factor: 2^20
		#ifdef FIXED_POINT
		input[i].re = scale32i(re,20);
		input[i].im = scale32i(im,20);		
		#else
		input[i].re = re;
		input[i].im = im;
		#endif
		//printf("%d %d\n", input[i].re, input[i].im);		
	}


#ifndef TIME_TEST
	//compute the fft
	if(inverse_flag == 1 ){
		inverse_fft(input, output, number_samples);
	}else{
		forward_fft(input, output, number_samples);
	}

	for( i = 0; i < number_samples; ++i){		
		// bit len: 32b
		// signed : 1b 
		// scale factor: 20 b
		#ifdef FIXED_POINT
		float re = unscale32i(output[i].re,20);
		float im = unscale32i(output[i].im,20);		
		#else
		float re = output[i].re;
		float im = output[i].im;
		#endif
		//printf("%f %f\n",re,im);
	}
	//print_complex_array(output,number_samples);
#else
	//compute the fft
	unsigned total_time = 0;
	unsigned timespent[2];
	int num_runs = 10;
	for(i = 0;i < num_runs ; ++i){
		timespent[0] = Microseconds();
		if(inverse_flag == 1 ){
			inverse_fft(input, output, number_samples);
		}else{
			forward_fft(input, output, number_samples);
		}
		timespent[1] = Microseconds();
		total_time += (timespent[1] - timespent[0]);
	}
	printf("%10d samples  %10d usecs\n",number_samples,total_time/num_runs);
#endif

	free(input);
	free(output);

	return 0;
}
