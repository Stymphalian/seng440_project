#ifndef JORDAN_FFT_H
#define JORDAN_FFT_H

typedef struct complex_t complex_t;
struct complex_t{
	unsigned int re;
	unsigned int im;
};


// all these methods are implemented in jordan_fft_util.h
unsigned Microseconds(void);
void print_spaces(int n);
void print_complex(complex_t c);
void print_complex_array(complex_t* c, int n);
void swap_complex_array(complex_t* c, int n);
//complex_t twiddle(int N,int k);
void complex_mult(complex_t* left, complex_t* right, complex_t* rs);
void complex_add(complex_t* left, complex_t* right, complex_t* rs);
int forward_fft(complex_t* input, complex_t* output, unsigned n);
int inverse_fft(complex_t* input, complex_t* output, unsigned n);

// input: pointer to an array of complex input samples
// ouptut: pointer to an array where the result can be placed.
// n : the number of samples.
// preconditions:
//		input and output are valid pointers
//		input and output are of size n
// 	n must be a power of 2.
// DFT is defined as X(n) = sum( x[n] * e^(-2jnk*pi/N ) for n = 0 to N-1
int _fft(complex_t* input, complex_t* output, unsigned n);

#endif
