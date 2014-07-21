#include <stdio.h>
#include <string.h>
#include "kiss_fft.h"


void run(int n,kiss_fft_cpx* input){
	kiss_fft_cfg c = kiss_fft_alloc(n, 0,0,0);
	kiss_fft_cpx output[n];
	kiss_fft(c, input, output);

	int i = 0;
	for( i = 0 ; i < n ; ++i){
		printf("%f %f\n", input[i].r, input[i].i);
	}
	for( i = 0;i < n; ++i){
		printf("%f %f\n", output[i].r, output[i].i);
	}
	printf("----\n");
	free(c);
}

int main(int argc, char** argv){
	if( argc != 3 ){
		printf("Usage: %s [num samples] [direction]\n",argv[0]);
		return 0;
	}

	int i = 0 ;
	int fft_size = atoi(argv[1]);
	int is_inverse_fft = atoi(argv[2]);

	kiss_fft_cfg fft_context = kiss_fft_alloc(fft_size,is_inverse_fft, 0,0);
	kiss_fft_cpx input[fft_size];
	kiss_fft_cpx output[fft_size];

	for(i = 0;i < fft_size; ++i){
		if(2 != scanf("%f %f",&input[i].r, &input[i].i) ){
			printf("Not enough samples in the input file\n");
			return 1;
		}
	}

	kiss_fft(fft_context,input,output);
	free(fft_context);

	if( is_inverse_fft){
		for(i =0;i < fft_size; ++i){
			printf("%.20f %.20f\n", output[i].r/fft_size, output[i].i/fft_size);
		}
	}else{
		for( i = 0;i < fft_size; ++i){
			printf("%.20f %.20f\n", output[i].r, output[i].i);
		}
	}
	return 0;
}
