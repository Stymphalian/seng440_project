#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

// Stolen directly from the hello_fft.c  file
unsigned Microseconds(void){
#ifdef _WIN64
	return 0;
#elif _WIN32
	return 0;
#else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*1000000 + ts.tv_nsec/1000;
#endif
}

void print_spaces(int n){
	if( n == 1) {return;}
	int i = 0;
	for( i = 0;i < n; ++i){
		printf(" ");
	}
}

void print_complex(complex_t c){
	//printf("%.20f %.20f\n", c.re, c.im);
	printf("%u %u\n", c.re, c.im);
}
void print_complex_array(complex_t* c, int n){
	unsigned i ;
	for( i = 0;i < n; ++i){
		print_complex(c[i]);
	}
}

// takes an array of complex_t numbers and swaps 
// their real and imaginary components.
void swap_complex_array(complex_t* c,int n){
	int i ;
	float temp;
	for( i = 0;i < n; ++i){
		temp = c[i].re;
		c[i].re = c[i].im;
		c[i].im = temp;
	}
}


// multiplies left*right and places the result in left
void complex_mult(complex_t* left, complex_t* right,complex_t* rs){
	rs->re = left->re*right->re - left->im*right->im;
	rs->im = left->im*right->re + left->re*right->im;
}

// add two complex numbers together put the result in left
void complex_add(complex_t* left, complex_t* right,complex_t* rs){
	rs->re = left->re + right->re;
	rs->im = left->im + right->im;
}


// Forward and Inverse FFT
int forward_fft(complex_t* input, complex_t* output, unsigned int n){
	if(!input || !output){return 0;}
	return _fft(input, output,n);
}
int inverse_fft(complex_t* input, complex_t* output, unsigned int n){
	if(!input || !output){return 0;}
	swap_complex_array(input,n);
	int rs= _fft(input, output,n);
	swap_complex_array(input,n);

	int i = 0;
	swap_complex_array(output,n);
	for( i = 0; i < n; ++i){
		output[i].re /= n;
		output[i].im /= n;	
	}
	return rs;
}
