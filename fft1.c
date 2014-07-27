#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "fft.h"


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

// input: pointer to an array of complex input samples
// ouptut: pointer to an array where the result can be placed.
// n : the number of samples.
// step: what level are we currently at in the fft
// preconditions:
//		input and output are valid pointers
//		input and output are of size n
// 	n must be a power of 2.
// DFT is defined as X(n) = sum( x[n] * e^(-2jnk*pi/N ) for n = 0 to N-1
static int _fft_run(complex_t* input, complex_t* output, int n, int step, int offset){
	if( n == 1){
		output[0] = input[0];
		return 1;
	}

	complex_t even_output[n/2];
	complex_t odd_output[n/2];
	_fft_run(input, even_output, n/2, 2*step,offset);
	_fft_run(input+step, odd_output, n/2, 2*step, offset+1);

	int i = 0;
	complex_t Y_k,Z_k,W;
	for( i = 0;i < n/2; ++i){
		Y_k = even_output[i];
		Z_k = odd_output[i];	

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
	return  _fft_run(input, output, n, 1, 0);
}
