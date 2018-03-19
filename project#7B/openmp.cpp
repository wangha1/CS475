#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

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
	
	omp_set_num_threads(8);
	
	double maxMegaMults = 0.;
	double sumMegaMults = 0.;
	
	
	for( int i = 0; i < NUMTRY; i++){
		double time0 = omp_get_wtime( );
		
#pragma omp parallel for
		for( int shift = 0; shift < Size; shift++ ){
			float sum = 0.;
			for( int i = 0; i < Size; i++ ){
				sum += Array[i] * Array[i + shift];
			}
			Sums[shift] = sum;	// note the "fix #2" from false sharing if you are using OpenMP
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
