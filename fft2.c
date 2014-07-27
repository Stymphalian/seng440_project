#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>


// Twiddle factors W_N defined as e^(-j2*pi*nk/N)
// N: is the base twiddle factor
// power: is the 'nk' portion of twiddle
// e^ja = cos(a) + jsin(a)
static complex_t twiddle(int N, int k){
	complex_t rs;
	rs.re = cos((2*M_PI*k)/N);
	rs.im = -sin((2*M_PI*k)/N);
	return rs;
}

static int bit_len(int n){
	int i =0;
	for(i = 0;i < 32; ++i){
		if( (n & (1 << i)) ){
			break;
		}
	}
	return i+1;
}

static unsigned bit_swap(unsigned num, int n){
	unsigned rs = 0;
	int i ;
	for(i = 0;i < n; ++i){
		if( (num & (1<<i)) == (1<<i) ){
			rs += (1<<(n-i-1));
		}
	}
	return rs;
}

static void jig_input(complex_t* input, complex_t* output, int n){
	int i = 0;
	int size = bit_len(n);
	--size;
	for( i = 0; i < n ;++i){
		output[bit_swap(i,size)] = input[i];
	}	
}

static int _fft_run(complex_t* output, int n){
	if( n == 1) {return 1;}
	
	_fft_run(output, n/2);
	_fft_run(output + n/2,n/2);

	int i = 0;
	complex_t Y_k,Z_k,W;

	for( i = 0;i < n/2; ++i){
		Y_k = output[i];
		Z_k = output[i +n/2];	

		W = twiddle(n, i);
		complex_t temp;
		complex_mult(&W,&Z_k,&temp);

		output[i].re = Y_k.re + temp.re;
		output[i].im = Y_k.im + temp.im;

		output[i + n/2].re = Y_k.re - temp.re;
		output[i + n/2].im = Y_k.im - temp.im;
	}
	return 1;
}

int _fft(complex_t* input, complex_t* output, unsigned n){
	jig_input(input,output,n);	
	_fft_run(output, n);
	return 0;
}
