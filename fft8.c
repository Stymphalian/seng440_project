//#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// @Purpose:
// Structure representing a complex number
// re = real component
// im = imaginary component
typedef struct complex_t complex_t;
struct complex_t{
	#ifdef FIXED_POINT
	int re;
	int im;
	#else
	float re;
	float im;
	#endif
};


// @Purpose:
// 	Structure of the fft_t context
// 	reference to the input array
//		reference to the output buffer
// 	reference to the lut of the twiddle factors.
typedef struct fft_t fft_t;
struct fft_t{
	complex_t* input;
	complex_t* output;
	complex_t* twiddles;
	unsigned n;	
};


// @Purpose:
//		Function to calculate the twiddle factors 
//		given a size 'N' and index 'k'
// @Summary
// 	Twiddle factors W_N defined as e^(-j2*pi*nk/N)
// 	N: is the base twiddle factor
// 	power: is the 'nk' portion of twiddle
// 	e^ja = cos(a) + jsin(a)
// @fixed-point-representation:
// 	bit-width: 32 bit
// 	signed: 1 bit
// 	-2.0 ..... 2.0 : 0 bit for decimal
// 	scaling factor: 2^32 / 2^1 = 2^30
// 	NOTE:  I use a scale factor of 2^30 because 
//		there is some weird behaviour when it is 31
static complex_t twiddle(unsigned N, unsigned k){
	complex_t rs;
	//double re,im;
	float re = cos((2*M_PI*k)/N);
	float im = -sin((2*M_PI*k)/N);			

	// fixed point arithmetic
	rs.re = re * (1 << 30);
	rs.im = im * (1 << 30);
	return rs;	
}

// @Purpose:
// 	calculate the power of two that this number represents.
//		in other words calculate the log_2 of n
// @Precond:
//		n is a power of 2.
static unsigned int log_2(register unsigned int n){	
	unsigned i =0;
	for(i = 0;i < 32; ++i){
		if( (n & (1 << i)) ){
			break;
		}
	}
	return i+1;
}


// @Purpose
//	jig the input
// copying input elements into the output buffer
// the output index is the bit_reversal of the input index
// The bit reversals allows use to perform the fft with an inplace array.
// 
// SOURCE: http://aggregate.org/MAGIC/#Bit%20Reversal
// e.g.
// 0000 -> 0000
// 0001 -> 1000
// 0010 -> 0100
// 0011 -> 1100
// ...
// 0111 -> 1110
// 1000 -> 0001
static void jig_input(complex_t* input, complex_t* output, unsigned n){
	register unsigned i = 0;
	register unsigned x;
	unsigned shift_amount = 32-log_2(n)+1;
	
	for( i = 0; i < n ;++i){
		// in-line of the bit-reversal32 code
		x = i;
		x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
		x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
		x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
		x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));		

		// place the input sample at the bit-reversed index in
		// in the output buffer.
		output[((x >> 16) | (x << 16)) >> shift_amount] = input[i];
	}	
}


// @Purpose
// 	Initialize a fft_t context. 
// 1. Jig the input into the output buffer, by placing the input
//		samples at the 'bit-reverse' index.
// 2. Pre-calculate the necessary twiddle factors
// @Return:
//		1 for success
// 	0 for failure (due to not enough available memory to malloc)
static int init_context(fft_t* context,complex_t* input, complex_t* output, unsigned int n){
	context->input = input;
	context->output = output;
	context->n = n;
	jig_input(input, output, n);	

	context->twiddles = malloc(sizeof(complex_t)* (n/2));
	if( context->twiddles == NULL){
		return 0;
	}

	// We only need to calculate n/2 twiddles
	// because the butterfly at the highest level only uses
	// up to the n/2 th twiddle factor.
	int i = 0;
	for( i = 0;i < n/2; ++i){
		context->twiddles[i] = twiddle(n,i);
	}
	return 1;
}

// @Purpose: 
//		Free up any memmory taken up by the context
// Frees up the context->twiddle memory
static void destroy_context(fft_t* context){
	if(!context){return;}
	free(context->twiddles);	
	context->twiddles = NULL;
}


// @Purpose:
//		The main calculation of the fft.
// @precond:
//		context is not null
//		output is not null
// 	n is a power of two. 
//		n < 2^17
// @side-effects:
//		Calculates the fft and places the reult into the 'output' buffer.
void _fft2(fft_t* context,complex_t* output,unsigned n){	
	unsigned level = 0; 						// loop counter, this is the len of the block size in powers of two
	unsigned num_levels = log_2(n); 		// log(n)
	unsigned block_size = 2;				// starting block_size is 2	
	unsigned num_blocks = 0;				// keep track of how many blocks are on this level
	unsigned segment = 0;					// loop counter, used to identify the paritcular segment out of all the blocks currently being processed
	complex_t * out;							// temporary pointer to the output buffer. used to reduce number of bit-shifts + additions.
	complex_t Y_k;								// Keep a copy of Y_k
	complex_t Z_k;								// pointer to the Z_k portion of the butterfly
	complex_t W;								// pointer to the twiddle factor
	int temp_re,temp_im;						// temp var to hold Z_k*W
	unsigned i;									// loop counter


	// block-size == 1 is already handled by the jig-input from the
	// fft_context initialization.
	for( level = 1; level <= num_levels; ++level){
		num_blocks = n >> level;
		segment = 0;		
		
		for(segment = 0; segment < num_blocks; ++segment){		
			out = output + (segment<< level);

			for( i = 0; i< (block_size >> 1); ++i){
				//Y_k = out[i];
				Y_k = out[i];
				Z_k = out[i + (block_size >> 1)];	
				
				// W = 2^16 b signed
				W = context->twiddles[(n >> level)*i];				
				
				// complex_multiplication
				// Z = 2^16 signed
				// scaling factor 2^16 * 2^30 --> 2^46 --> 2^16
				temp_re = ((long long int)W.re*Z_k.re >> 30) - ((long long int)W.im*Z_k.im >> 30);
				temp_im = ((long long int)W.im*Z_k.re >> 30) + ((long long int)W.re*Z_k.im >> 30);				

				
				// place into output buffer.
				// out = 2^16  + 2^16 -> 2^16
				out[i].re = (Y_k.re) + (temp_re); // Y_k = 2^16, temp_re = 2^16 -> 2^16
				out[i].im = (Y_k.im) + (temp_im); // Y_k = 2^16, temp_im = 2^16 -> 2^16
				
				// place into output buffer.
				out[i + (block_size >>1 )].re = (Y_k.re) - (temp_re); // Y_k = 2^16, temp_re = 2^16 -> 2^16
				out[i + (block_size >>1 )].im = (Y_k.im) - (temp_im); // Y_k = 2^16, temp_im = 2^16 -> 2^16
			}
		}
		block_size  = block_size << 1;		
	}
}


// @Purpose:
//		Interface method. The forward_fft, and inverse_fft methods
// 	will call this function to perform the fft.
// @precond: 
//		input is a valid pointer to 'n' input samples
//		output is a valid pointer to n-sized buffer
//		n is a power of two
// @return: 
//		1 for success, 0 for failure.
int _fft(complex_t* input, complex_t* output, unsigned n){		
	fft_t context;
	if( init_context(&context, input, output, n) == 0) {
		return 0;
	}
	_fft2(&context,context.output,context.n);	
	destroy_context(&context);
	return 1;
}
