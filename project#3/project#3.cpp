#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMPAD		6
#define NUMT		4
#define NUMTRIES        10

struct s
{
	float value;
	int pad[NUMPAD];
} Array[4];

int main(){
	omp_set_num_threads( NUMT );

	double maxMegaMults = 0.;
	double sumMegaMults = 0.;
	
	const int SomeBigNumber = 100000000;	// keep < 2B
	for( int t = 0; t < NUMTRIES; t++ )
	{
		double time0 = omp_get_wtime( );
#pragma omp parallel for
		for( int i = 0; i < 4; i++ )
		{
			unsigned int seed = 0;		// automatically private
			float TEMP  = Array[i].value;
			for( unsigned int j = 0; j < SomeBigNumber; j++ )
			{
				TEMP = TEMP + (float)rand_r( &seed );
			}
			Array[i].value = TEMP;
		}
		double time1 = omp_get_wtime( );
		long int numMuled = 4 * SomeBigNumber;
		double megaMults = (double)numMuled/(time1-time0)/1000000.;
		sumMegaMults += megaMults;
		if( megaMults > maxMegaMults )
			maxMegaMults = megaMults;
	}
	double avgMegaMults = sumMegaMults/(double)NUMTRIES;
	printf( "   Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
	printf( "Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults );
	
	return 0;
}
