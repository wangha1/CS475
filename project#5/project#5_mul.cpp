#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "simd.p5.h"

#define NUMTRY	200
//#define ARRAY_SIZE	1000


float A[ARRAY_SIZE];
float B[ARRAY_SIZE];
float C[ARRAY_SIZE];


float rand_float(unsigned int *seedp, float low, float high) {
	
	float r = (float) rand_r(seedp);
	
	return(low + r * (high - low) / (float) RAND_MAX);
}

void fill(float *array, int len) {
	
	unsigned int seed = 0;
	
	for(int i = 0; i < len; i++)
		array[i] = rand_float(&seed, -1., 1.);
}

void mul(float *A, float *B, float *C, int len) {
	
	for(int i = 0; i < len; i++) {
		
		C[i] = A[i]*B[i];
		
	}
	
}

int main(){
	fill(A, ARRAY_SIZE);
	fill(B, ARRAY_SIZE);
	
	double time1, time2;
	float avetime_cpp, avetime_simd;
	
	for(int i = 0; i < NUMTRY; i++){
		time1 = omp_get_wtime();
		mul(A,B,C,ARRAY_SIZE);
		time2 = omp_get_wtime();
		
		avetime_cpp += (time2 - time1);
		
	}
	avetime_cpp = avetime_cpp / NUMTRY;
	
	for(int i = 0; i < NUMTRY; i++){
		time1 = omp_get_wtime();
		SimdMul(A,B,C,ARRAY_SIZE);
		time2 = omp_get_wtime();
		
		avetime_simd += (time2 - time1);
		
	}
	avetime_simd = avetime_simd / NUMTRY;
	
	float speed_up = avetime_cpp / avetime_simd;
	
	printf("%d %f\n", ARRAY_SIZE, speed_up);
	
	return 0;
	
}
