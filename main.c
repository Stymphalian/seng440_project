#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv){
	if( argc != 3){
		printf("Usage: %s [num_samples] [forward|inverse] \n", argv[0]);
		return 0;
	}

	// determine if we should be doing a forward or inverse fft.
	int number_samples = atoi(argv[1]);
	int inverse_flag = atoi(argv[2]);
	int i = 0;
	// complex_t input[number_samples];
	//	complex_t output[number_samples];


	complex_t* input = malloc(sizeof(complex_t)*number_samples);
	complex_t* output = malloc(sizeof(complex_t)*number_samples);
	if(!input || !output){
		fprintf(stderr,"Unable to malloc enough memory\n");
		free(input);
		free(output);
		return 0;
	}
	double re,im;
	for(i =0;i < number_samples; ++i){		
		if(EOF == scanf("%f %f",&re,&im) ){
			printf("Not enough samples. Expected %d samples, but only %d read\n",
						number_samples,i);
			return 0;
		}
		input[i].re = re;
		input[i].im = im;
	}


#ifndef TIME_TEST
	//compute the fft
	if(inverse_flag == 1 ){
		inverse_fft(input, output, number_samples);
	}else{
		forward_fft(input, output, number_samples);
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
