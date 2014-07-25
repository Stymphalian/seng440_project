#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>


void print_bits(unsigned long long x){
	int i= 0;
	for( i = 63;  i > 0; --i){
		if( (x & ( 1<< i)) == 1 << i ){
			printf("1");
		}else{
			printf("0");
		}
	}
}

void baz(){
	float a = 0.00000000000000024492;
	int na = scale32i(a,20);
	printf("a float = %f\n", a);
	printf("na fixed = %d\n", na);
	printf("na float = %f\n", unscale32i(a,20));
	exit(0);
}

void bar(){	

	float fYk_re = 0.5;
	float fYk_im = -1.0;
	float fZk_re = 1.25;
	float fZk_im = -1.0;
	float twid_re = 1.0;
	float twid_im = 0.0;	

	int Yk_re = scale32i(fYk_re,20);
	int Yk_im = scale32i(fYk_im,20);
	int Zk_re = scale32i(fZk_re,20);
	int Zk_im = scale32i(fZk_im,20);
	int W_re = scale32i(twid_re,30);
	int W_im = scale32i(twid_im,30);

	printf("%d\n",Yk_re);
	printf("%d\n",Yk_im);
	printf("%d\n",Zk_re);
	printf("%d\n",Zk_im);
	printf("%d\n",W_re );
	printf("%d\n",W_im );
	printf("-------\n");	


	printf("twid_re*fZk_re = %f\n",twid_re*fZk_re);
	printf("twid_im*fZk_im = %f\n",twid_im*fZk_im);
	printf("temp_re = %f\n",twid_re*fZk_re - twid_im*fZk_im );	
	printf("------\n");

	int temp1 = ((long long int)W_re*Zk_re) >> 32;	
	int temp2 = ((long long int)W_im*Zk_im) >> 32;		
	int temp_re = temp1 - temp2;
	printf("temp1 decimal = %d\n", temp1);
	printf("temp2 decimal = %d\n", temp2);
	printf("temp1 = %f\n",unscale32i(temp1,18));
	printf("temp2 = %f\n",unscale32i(temp2,18))	;
	printf("temp_re = %f\n",unscale32i(temp_re,18));
	printf("-----\n");

	printf("twid_im*fZk_re = %f\n",twid_im*fZk_re);
	printf("twid_re*fZk_im = %f\n",twid_re*fZk_im);
	printf("temp_im = %f\n",twid_im*fZk_re + twid_re*fZk_im );		
	printf("------\n");

	temp1 = ((long long int)W_im*Zk_re) >> 32;
	temp2 = ((long long int)W_re*Zk_im) >> 32;
	int temp_im = temp1 + temp2;
	printf("temp1=%f\n",unscale32i(temp1,18));
	printf("temp2=%f\n",unscale32i(temp2,18))	;
	printf("temp_im=%f\n",unscale32i(temp_im,18));
	printf("-----\n");


	printf("out[i].re = %f\n", fYk_re + twid_re*fZk_re - twid_im*fZk_im);
	printf("out[i].im = %f\n", fYk_im + twid_im*fZk_re + twid_re*fZk_im);
	printf("------\n");

	temp1 = (Yk_re ) + (temp_re << 2);
	temp2 = (Yk_im ) + (temp_im << 2);
	printf("out[i].re decimal = %d\n", temp1);
	printf("out[i].im decimal = %d\n", temp2);
	printf("out[i].re = %f\n", unscale32i(temp1,20));
	printf("out[i].im = %f\n", unscale32i(temp2,20));
	printf("------\n");


	printf("out[i + n/2].re = %f\n", fYk_re - (twid_re*fZk_re - twid_im*fZk_im) );
	printf("out[i + n/2].im = %f\n", fYk_im - (twid_im*fZk_re + twid_re*fZk_im) );
	printf("------\n");

	temp1 = (Yk_re ) - (temp_re << 2);
	temp2 = (Yk_im ) - (temp_im << 2);
	printf("out[i + n/2].re decimal = %d\n", temp1);
	printf("out[i + n/2].im decimal = %d\n", temp2);
	printf("out[i + n/2].re = %f\n", unscale32i(temp1,20));
	printf("out[i + n/2].im = %f\n", unscale32i(temp2,20));
	printf("------\n");




	printf("%f\n", unscale32i(Yk_re,20));
	printf("%f\n", unscale32i(Yk_im,20));
	printf("%f\n", unscale32i(Zk_re,20));
	printf("%f\n", unscale32i(Zk_im,20));
	printf("%f\n", unscale32i(W_re ,31));
	printf("%f\n", unscale32i(W_im ,31));



	//long long int t = (long long int)W_re*(long long int)Zk_re;
	//printf("t = %I64d\n",t);	
	//printf("t = %I64d\n",t >> 32);
	//int t2 = (int)(t >> 32);
	//printf("t2 fixed = %d\n",t2);
	////printf("t2 float = %f\n",unscale32i(t2,20));
	//printf("t2 float = %f\n",unscale32i(t2,19));
	exit(0);
}
void foo(){	
	float a = 4.000;
	float na = -4.00;
	int p = 0;
	int np = 0;
	int scale = 20;

	p = scale32i(a,scale);		
	printf("float a = %f\n", a);
	printf("float p = %f\n", a*(1<<scale));
	printf("fixed a = %d\n", p);	
	printf("float p = %f\n", unscale32i(p,scale));
	
	np = scale32i(na,scale);	
	printf("float na = %f\n", na);
	printf("float np = %f\n", na*(1<<scale));
	printf("fixed na = %d\n", np);	
	printf("float np = %f\n", unscale32i(np,scale));

	print_bits(p);
	printf("\n");
	print_bits(np);
	printf("\n");
	exit(0);
}


int main(int argc, char** argv){
	//baz();
	//bar();
	//foo();	
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
		printf("%f %f\n",re,im);
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
