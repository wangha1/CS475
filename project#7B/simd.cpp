#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include "simd.p5.h"

#define SIZE   32768
#define NUMTRY	50


float Array[2*SIZE];
float  Sums[1*SIZE];


int main(){
	int     Size;
	FILE *  fp;
	int     i;
	
	
	fp = fopen( "signal.txt", "r" );
	if( fp == NULL ){
		fprintf( stderr, "Cannot open file 'signal.txt'\n" );
		exit( 1 );
	}
	fscanf( fp, "%d", &Size );
	Size = SIZE;
	for( i = 0; i < Size; i++ ){
		fscanf( fp, "%f", &Array[i] );
		Array[i+Size] = Array[i];		// duplicate the array
	}
	fclose( fp );
	
	
	double maxMegaMults = 0.;
	double sumMegaMults = 0.;
	
	
	for( int i = 0; i < NUMTRY; i++){
		double time0 = omp_get_wtime( );
		
		for( int shift = 0; shift < Size; shift++ ){
			Sums[shift] = SimdMulSum( &Array[0], &Array[0+shift], Size );
		}
		
		double time1 = omp_get_wtime( );
		double megaMults = (double)(Size * Size)/(time1-time0)/1000000.;
		sumMegaMults += megaMults;
		if( megaMults > maxMegaMults )
			maxMegaMults = megaMults;
	}
	
	double avgMegaMults = sumMegaMults/NUMTRY;
	printf( "   Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
	printf( "Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults );
	
	/*for(int i = 1; i < 512; i++){
		printf("%f\n", Sums[i]);
	 }*/
	
	return 0;
}
