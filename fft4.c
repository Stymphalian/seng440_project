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
	int n;	
};

// Twiddle factors W_N defined as e^(-j2*pi*nk/N)
// N: is the base twiddle factor
// power: is the 'nk' portion of twiddle
// e^ja = cos(a) + jsin(a)
static complex_t twiddle(int N, int k){
	complex_t rs;
	rs.re = cos((2*M_PI*k)/N);
	rs.im = -sin((2*M_PI*k)/N);
	//printf("N=%d,k=%d === (%f,%f)\n",N,k, rs.re, rs.im);	
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

static int init_context(fft_t* context,complex_t* input, complex_t* output, int n){
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


int _fft2(fft_t* context,complex_t* output,int n){	
	int level = 0;
	int num_levels = bit_len(n);
	int block_size = 2;
	for( level = num_levels-1; level != 0 ; --level){

		int num_blocks = n / block_size;
		int segment = 0;
		//printf("level %d\n", level);
		//printf("num_blocks = %d\n", num_blocks);
		//printf("block_size = %d\n", block_size);

		complex_t Y_k,Z_k,W;
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
				complex_t temp;
				complex_mult(&W,&Z_k,&temp);

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

int _fft(complex_t* input, complex_t* output, unsigned n){
	int rs = 0;
	fft_t context;
	init_context(&context, input, output, n);
	rs = _fft2(&context,context.output,context.n);	
	destroy_context(&context);
	return rs;
}
