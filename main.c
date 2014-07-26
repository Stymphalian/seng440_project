#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

void foo();
int main(int argc, char** argv){
	//foo();
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
		input[i].re = scale32i(re,16);
		input[i].im = scale32i(im,16);		
		#elif SHORT_FIXED_POINT
		input[i].re = scale16i(re,10);
		input[i].im = scale16i(im,10);		
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
		float re = unscale32i(output[i].re,16);
		float im = unscale32i(output[i].im,16);
		#elif SHORT_FIXED_POINT
		float re = unscale16i(output[i].re,10);
		float im = unscale16i(output[i].im,10);
		#else
		float re = output[i].re;
		float im = output[i].im;
		#endif

		#ifdef PRINT_OUTPUT
		printf("%f %f\n",re,im);
		#endif
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

void foo(){
	float a = 0.5;
	float b = 0.0000008;
	float c = a*b;

	int int_a = scale32i(a,20);
	int int_b = scale32i(b,30);
	int int_c = ((long long int)int_a*int_b >> 30);

	float rs_a = unscale32i(int_a,20);
	float rs_b = unscale32i(int_b,30);
	float rs_c = unscale32i(int_c,20);

	printf("a = %f\n", a);
	printf("b = %f\n", b);
	printf("c = %f\n", c);

	printf("int_a = %d\n", int_a);
	printf("int_b = %d\n", int_b);
	printf("int_c = %d\n", int_c);

	printf("rs_a = %f\n", rs_a);
	printf("rs_b = %f\n", rs_b);
	printf("rs_c = %f\n", rs_c);
	

	exit(0);
}