#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

typedef struct complex_t complex_t;
struct complex_t{
	float re;
	float im;
};

// Stolen directly from the hello_fft.c  file
unsigned Microseconds(void){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

void print_spaces(int n){
	if( n == 1) {return;}
	int i = 0;
	for( i = 0;i < n; ++i){
		printf(" ");
	}
}

void print_complex(complex_t c){
	printf("%.20f %.20f\n", c.re, c.im);
}
void print_complex_array(complex_t* c, int n){
	int i ;
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

// Twiddle factors W_N defined as e^(-j2*pi*nk/N)
// N: is the base twiddle factor
// power: is the 'nk' portion of twiddle
// e^ja = cos(a) + jsin(a)
complex_t twiddle(int N, int k){
	complex_t rs;
	rs.re = cos((2*M_PI*k)/N);
	rs.im = -sin((2*M_PI*k)/N);
	return rs;
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

// input: pointer to an array of complex input samples
// ouptut: pointer to an array where the result can be placed.
// n : the number of samples.
// step: what level are we currently at in the fft
// preconditions:
//		input and output are valid pointers
//		input and output are of size n
// 	n must be a power of 2.
// DFT is defined as X(n) = sum( x[n] * e^(-2jnk*pi/N ) for n = 0 to N-1
int _fft2(complex_t* input, complex_t* output, int n, int step, int offset){
	if( n == 1){
		output[0] = input[0];
		return 1;
	}

	complex_t even_output[n/2];
	complex_t odd_output[n/2];
	_fft2(input, even_output, n/2, 2*step,offset);
	_fft2(input+step, odd_output, n/2, 2*step, offset+1);

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

int bit_len(int n){
	int i =0;
	for(i = 0;i < 32; ++i){
		if( (n & (1 << i)) ){
			break;
		}
	}
	return i+1;
}

unsigned bit_swap(unsigned num, int n){
	unsigned rs = 0;
	int i ;
	for(i = 0;i < n; ++i){
		if( (num & (1<<i)) == (1<<i) ){
			rs += (1<<(n-i-1));
		}
	}
	return rs;
}

void jig_input(complex_t* input, complex_t* output, int n){
	int i = 0;
	int size = bit_len(n);
	--size;
	for( i = 0; i < n ;++i){
		output[bit_swap(i,size)] = input[i];
	}	
}

int _fft_run(complex_t* output, int n){
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

int _fft(complex_t* input, complex_t* output, int n, int step, int offset){
	jig_input(input,output,n);	
	_fft_run(output, n);
	return 0;
}


int forward_fft(complex_t* input, complex_t* output, int n){
	if( !input || !output ) {return 0;}
	return _fft(input, output,n,1,0);
}

int inverse_fft(complex_t* input, complex_t* output, int n){
	if( !input || !output ) {return 0;}
	swap_complex_array(input,n);
	int rs = _fft(input, output, n,1,0);
	swap_complex_array(input,n);

	if( rs == 0){return 0;}
	int i = 0;
	swap_complex_array(output, n);
	for( i = 0; i < n; ++i){
		output[i].re /= n;
		output[i].im /= n;	
	}
	return rs;
}



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
	for(i =0;i < number_samples; ++i){
		if(EOF == scanf("%f %f", &input[i].re, &input[i].im) ){
			printf("Not enough samples. Expected %d samples, but only %d read\n",
						number_samples,i);
			return 0;
		}
	}


#ifndef TIME_TEST
	//compute the fft
	if(inverse_flag == 1 ){
		inverse_fft(input, output, number_samples);
	}else{
		forward_fft(input, output, number_samples);
	}
	print_complex_array(output,number_samples);
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
