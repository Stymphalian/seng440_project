#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>


typedef struct complex_t complex_t;
struct complex_t{
	double re;
	double im;
};


void print_complex_array(complex_t* a, int n){
	int i = 0;
	for(i = 0;i <n; ++i){
		printf("%.20f %.20f\n", a[i].re, a[i].im);
	}
}

void re_sin_wave(complex_t* output,int size,float scale){
	int i = 0;
	for(i = 0;i < size; ++i){
		output[i].re = sin(2*M_PI*i/size)*scale;
		output[i].im = 0;
	}
}
void re_im_sin_wave(complex_t* output,int size,float scale){
	int i = 0;
	for( i = 0;i < size; ++i){
		output[i].re = sin(2*M_PI*i/size)*scale;
		output[i].im = cos(2*M_PI*i/size)*scale;
	}
}
void re_step(complex_t* output,int size,float scale){
	int i = 0;
	for(i = 0; i < size/2; ++i){
		output[i].re = 0;
		output[i].im = 0;
	}
	for( i = size/2; i < size; ++i){
		output[i].re = 1*scale;
		output[i].im = 0;
	}
}
void re_im_step(complex_t* output,int size,float scale){
	int i = 0;
	for(i = 0; i < size/2; ++i){
		output[i].re = 0;
		output[i].im = 1*scale;
	}
	for( i = size/2; i < size; ++i){
		output[i].re = 1*scale;
		output[i].im = 0;
	}
}
void re_rect(complex_t* output,int size,float scale){
	int i = 0;
	int segment = size/4;
	for(i = 0; i < segment; ++i){
		output[i].re = 0;
		output[i].im = 0;
	}
	for(i = segment; i < 3*segment; ++i){
		output[i].re = 1*scale;
		output[i].im = 0;
	}
	for(i =3*segment; i < size; ++i){
		output[i].re = 0;
		output[i].im = 0;
	}
}
void re_im_rect(complex_t* output,int size,float scale){
	int i = 0;
	int segment = size/4;
	for(i = 0; i < segment; ++i){
		output[i].re = 0;
		output[i].im = 1*scale;
	}
	for(i = segment; i < 3*segment; ++i){
		output[i].re = 1*scale;
		output[i].im = 0;
	}
	for(i =3*segment; i < size; ++i){
		output[i].re = 0;
		output[i].im = 1*scale;
	}
}

int main(int argc,char** argv){
	if( argc < 3){
		printf("usage: %s [num_samples] [type] [scale]\n",argv[0]);
		printf("num_samples = a power of two\n");
		printf("type:\n");
		printf("0 = real sin wave \n");
		printf("1 = real sin wave + imaginary cos wave\n");
		printf("2 = real step function\n");
		printf("3 = real step function + im step function\n");
		printf("4 = real rect function\n");
		printf("5 = real rect function + im rect function\n");
		return 0;
	}
	int size = atoi(argv[1]);
	int type = atoi(argv[2]);
	float scale = 1.0;
	if( argc >= 4){
		scale = atof(argv[3]);
	}


	complex_t output[size];
	switch(type){
		case(0): re_sin_wave(output,size,scale);break;
		case(1): re_im_sin_wave(output,size,scale);break;
		case(2): re_step(output,size,scale);break;
		case(3): re_im_step(output,size,scale);break;
		case(4): re_rect(output,size,scale);break;
		case(5): re_im_rect(output,size,scale);break;
		default:break;
	}

	print_complex_array(output,size);
	return 0;
}
