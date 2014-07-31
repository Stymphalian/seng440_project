#ifndef FFT_H
#define FFT_H

#define SCALE_FACTOR 12

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

// all these methods are implemented in fft_util.h
unsigned Microseconds(void);
// helpter methos used to translate between fixed and floating point representation
int scale32i(float original_value, int scale);
float unscale32i(int scaled_value, int scale);
// helper methods used to handle complex numbers,and for debugging
void print_spaces(int n);
void print_complex(complex_t c);
void print_complex_array(complex_t* c, int n);
void swap_complex_array(complex_t* c, int n);
void complex_mult(complex_t* left, complex_t* right, complex_t* rs);
void complex_add(complex_t* left, complex_t* right, complex_t* rs);
// interface methods used to call the forward,or inverse fft
int forward_fft(complex_t* input, complex_t* output, unsigned n);
int inverse_fft(complex_t* input, complex_t* output, unsigned n);
int forward_fft_gpu(complex_t* input, complex_t* output, unsigned n);


// _fft is implemented in the fft*.c file
// input: pointer to an array of complex input samples
// ouptut: pointer to an array where the result can be placed.
// n : the number of samples.
// preconditions:
//		input and output are valid pointers
//		input and output are of size n
// 	n must be a power of 2.
// DFT is defined as X(n) = sum( x[n] * e^(-2jnk*pi/N ) for n = 0 to N-1
int _fft(complex_t* input, complex_t* output, unsigned n);
fft_t _qfft(complex_t* input, complex_t* output, unsigned n);
void _qdestroy(fft_t* context);

#endif
