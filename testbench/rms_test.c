#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct complex_t complex_t;
struct complex_t{
 	double re;
	double im;
};

void print_complex_array(complex_t* a, int n){
	int i = 0;
	for( i =0;i < n; ++i){
		printf("%.20f %.20f\n", a[i].re, a[i].im);
	}
}

void root_mean_square(complex_t* error, int n ){
	int i = 0;
	complex_t temp;
	temp.re = 0;
	temp.im = 0;
	for( i = 0;i < n; ++i){
		temp.re += error[i].re*error[i].re;
		temp.im += error[i].im*error[i].im;	
	}
	temp.re /= n;
	temp.im /= n;
	
	double r = sqrt(temp.re*temp.re + temp.im*temp.im);
	printf("real rms=%.20f\n",r);
}

int main(int argc, char** argv){
	if(argc != 4){
		printf("Usage: %s [num_samples] [file1] [file2]\n", argv[0]);
		return 0;
	}

	int n = atoi(argv[1]);
	FILE* fin1 = fopen(argv[2],"r");
	FILE* fin2 = fopen(argv[3],"r");

	complex_t error[n];	

	int i = 0;
	complex_t temp1;
	complex_t temp2;
	for( i = 0;i < n; ++i){

		// these might need to be %f instead of %lf
		fscanf(fin1,"%lf %lf",&temp1.re, &temp1.im);
		fscanf(fin2,"%lf %lf",&temp2.re, &temp2.im);
	//	printf("%f %f, %f %f\n", temp1.re, temp1.im, temp2.re, temp2.im);

		// get the difference between the two complex numbers
		// real and imaginary parts
		error[i].re = fabs(temp1.re - temp2.re);
		error[i].im = fabs(temp1.im - temp2.im);
	}
	fclose(fin1);
	fclose(fin2);


	//print_complex_array(error, n);

	// root mean square
	root_mean_square(error,n);

	return 0;
}
