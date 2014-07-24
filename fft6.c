#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>


typedef struct fft_t fft_t;
struct fft_t{
	complex_t* input;
	complex_t* output;
	complex_t* twiddles;
	unsigned n;	
};

// Twiddle factors W_N defined as e^(-j2*pi*nk/N)
// N: is the base twiddle factor
// power: is the 'nk' portion of twiddle
// e^ja = cos(a) + jsin(a)
static complex_t twiddle(unsigned N, unsigned k){
	complex_t rs;
	rs.re = cos((2*M_PI*k)/N);
	rs.im = -sin((2*M_PI*k)/N);
	//printf("N=%d,k=%d === (%f,%f)\n",N,k, rs.re, rs.im);	
	return rs;
}

// determine the number of trailing zeroes for the number.
// Preconditions: n is a power of 2.
static unsigned int bit_len32(register unsigned int n){	
	unsigned i =0;
	for(i = 0;i < 32; ++i){
		if( (n & (1 << i)) ){
			break;
		}
	}
	return i+1;
}

// SOURCE: http://aggregate.org/MAGIC/#Bit%20Reversal
// e.g.
// 0000 -> 0000
// 0001 -> 1000
// 0010 -> 0100
// 0011 -> 1100
// 0100 -> 0010
// 0101 -> 1010
// 0110 -> 0110
// 0111 -> 1110
// 1000 -> 0001
static unsigned int bit_reverse32(register unsigned int x)
{
	x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
	x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
	x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
	return((x >> 16) | (x << 16));
}

// jig the input
// copying input elements into the output buffer
// the output index is the bit_reversal of the input index
// The bit reveral is used in order to make the butterfly operation possible
// for an in-place array.
static void jig_input(complex_t* input, complex_t* output, unsigned n){
	unsigned i = 0;
	//int size = bit_len(n);
	//--size;
	unsigned shift_amount = 32-bit_len32(n)+1;
	
	for( i = 0; i < n ;++i){
		//printf("%u --> %u\n", i , bit_reverse32(i) >> shift_amount);
		output[bit_reverse32(i)>>shift_amount] = input[i];
	}	
}


// Initialize a fft_t context
// 1. Jig the input
// 2. Pre-calculate the necessary twiddle factors
static int init_context(fft_t* context,complex_t* input, complex_t* output, unsigned int n){
	context->input = input;
	context->output = output;
	context->n = n;
	jig_input(input, output, n);	

	context->twiddles = malloc(sizeof(complex_t)* (n/2));
	int i = 0;
	for( i = 0;i < n/2; ++i){
		context->twiddles[i] = twiddle(n,i);
	}
	return 1;
}

static void destroy_context(fft_t* context){
	free(context->twiddles);	
}


int _fft2(fft_t* context,complex_t* output,unsigned n){	
	int level = 0;
	int num_levels = bit_len32(n);
	int block_size = 2;

	// num_levels -1 because we don't need to worry about block_size = 1 case.
	// It is already handled by the jig_input().
	for( level = num_levels-1; level != 0 ; --level){

		int num_blocks = n / block_size;
		int segment = 0;
		//printf("level %d\n", level);
		//printf("num_blocks = %d\n", num_blocks);
		//printf("block_size = %d\n", block_size);

		complex_t Y_k,Z_k,W;
		complex_t temp;
		complex_t* out;
		for(segment = 0; segment < num_blocks; ++segment){		
			out = output + segment*block_size;
			//printf("\t segment %d, abs_pos = %d\n", segment, segment*block_size);

			int i = 0;
			for( i = 0; i< block_size/2; ++i){
				//int poo = segment*block_size;
				//printf("\t\t %d,%d\n", i  + poo, i + block_size/2 + poo);

				Y_k = out[i];
				Z_k = out[i +block_size/2];	

				//W = twiddle(block_size, i);
				//printf("\t\t%d\n",(n/block_size)*i);
				W = context->twiddles[(n/block_size)*i];

				// complex_multiplication
				temp.re = W.re*Z_k.re - W.im*Z_k.im;
				temp.im = W.im*Z_k.re + W.re*Z_k.im;			
				
				out[i].re = Y_k.re + temp.re;
				out[i].im = Y_k.im + temp.im;

				out[i + block_size/2].re = Y_k.re - temp.re;
				out[i + block_size/2].im = Y_k.im - temp.im;
			}
		}
		block_size *= 2;
	}
	return 1;
}

int _fft(complex_t* input, complex_t* output, int n){
	int rs = 0;
	fft_t context;
	init_context(&context, input, output, n);
	rs = _fft2(&context,context.output,context.n);	
	destroy_context(&context);
	return rs;
}
