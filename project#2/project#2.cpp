#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define NUMT	         16
#define ARRAYSIZE       32 * 1024	// you decide
#define NUMTRIES        100	// you decide
#define CHUNKSIZE		1

float A[ARRAYSIZE];



float Ranf( float low, float high )
{
	float r = (float) rand();		// 0 - RAND_MAX
	
	return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int main()
{
#ifndef _OPENMP
	fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
	return 1;
#endif
	
	omp_set_num_threads( NUMT );
	fprintf( stderr, "Using %d threads\n", NUMT );
	
	double maxMegaMults = 0.;
	double sumMegaMults = 0.;
	
	for ( int i = 0; i< ARRAYSIZE; i++)
	{
		A[i] = Ranf(-1.f, 1.f);
	}
	
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );
#pragma omp parallel for schedule(dynamic,CHUNKSIZE)
		for( int i = 0; i < ARRAYSIZE; i++ )
		{
			float prod = 1.;
			for( int j = 0; j < i; j++)
			{
				prod *= A[j];
			}
		}
		
		double time1 = omp_get_wtime( );
		long int numMuled = (long int)ARRAYSIZE * (long int)(ARRAYSIZE+1) / 2;  // count of how many multiplications were done:
		double megaMults = (double)numMuled/(time1-time0)/1000000.;
		sumMegaMults += megaMults;
		if( megaMults > maxMegaMults )
			maxMegaMults = megaMults;
	}
	
	double avgMegaMults = sumMegaMults/(double)NUMTRIES;
	printf("Chunk size = %d\n", CHUNKSIZE);
	printf("Schedule type = dynamic\n ");
	printf( "   Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
	printf( "Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults );
	
	// note: %lf stands for "long float", which is what printf calls a "double"
	//        %d stands for "decimal integer", not "double"
	
	return 0;
}
