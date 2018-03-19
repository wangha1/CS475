//This program print the data I need and I can save that into a file call "result" (./a.out > result)
//then I have another python program to interpret the result into excel for me to graph
//funny things can be seen in another cpp file.




#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int	NowYear;		// 2017 - 2022
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumWolf;


const float GRAIN_GROWS_PER_MONTH =		8.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;
const float ONE_WOLF_EATS_PER_MONTH =		1;


const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				50.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;





float SQR( float x )
{
	return x*x;
}

void GrainDeer(){
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tempEat = (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		// DoneComputing barrier:
#pragma omp barrier
		
		NowHeight -= tempEat;
		
		if(NowNumDeer < NowHeight){
			NowNumDeer++;
		}
		else if(NowNumDeer > NowHeight){
			NowNumDeer--;
		}
		
		if(NowNumDeer < 0){
			NowNumDeer = 0;
		}


		
		// DoneAssigning barrier:
#pragma omp barrier
		
		// DonePrinting barrier:
#pragma omp barrier
		
	}
}

void Grain(){
	while( NowYear < 2023 )
	{
		
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
		
		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		unsigned int seed = 0;
		int random = rand() % 20 - 10;
		NowTemp = temp + random;
		
		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		random = rand() % 8 - 4;
		NowPrecip = precip + random;
		if( NowPrecip < 0. )
			NowPrecip = 0.;
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
		
		float tempGrow = tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		
		// DoneComputing barrier:
#pragma omp barrier
		
		NowHeight += tempGrow;
		
		// DoneAssigning barrier:
#pragma omp barrier
		
		
		// DonePrinting barrier:
#pragma omp barrier
	}
}

void Watcher(){
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		
		// DoneComputing barrier:
#pragma omp barrier
		
		// DoneAssigning barrier:
#pragma omp barrier
		if(NowHeight <= 0){
			NowHeight = 0;
		}
		printf("%f %f %f %d %d\n", NowTemp, NowPrecip, NowHeight, NowNumDeer, NowNumWolf);
		//printf("Year: %d\n", NowYear);
		//printf("Month: %d\n", NowMonth + 1);
		//printf("Deer remaining at the end of month: %d\n", NowNumDeer);
		//printf("Wolf remaining at the end of month: %d\n", NowNumWolf);
		//printf("High at the end of month: %f\n", NowHeight);
		//printf("-------------------------------------------------------------------------\n");
		// DonePrinting barrier:
		NowMonth++;
		if(NowMonth > 11){
			NowYear++;
			NowMonth = 0;
		}
#pragma omp barrier
		
	}
}

void MyAgent(){
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tempGrow = 0.;
		int tempDeer = 0;
		int tempWolf = 0;
		
		if(NowHeight >= 8.0){
			tempGrow = -0.5;
			//printf("Your grain higher than 8 inch, because it became easier to destoried by wind, result 0.5 less inch grown this month\n");
		}
		
		if(NowNumDeer >= 5 && NowNumWolf == 0){
			tempDeer += -1;
			tempWolf += 1;
			//printf("Too much deer result a wolf in your field, eaten 1 deer this month\n");
		}
		
		if(NowNumWolf > NowNumDeer && NowNumWolf != 0){
			tempWolf -= NowNumWolf - NowNumDeer;
			//printf("No enough deer for wolf, reduce wolf number to the number of deer\n");
		}
		
		if(NowNumDeer >= NowNumWolf && NowNumWolf != 0){
			tempWolf += 1;
			//printf("A lot deer for wolf, increased wolf number by 1\n");
		}
		
		if((NowHeight - NowNumDeer) >= NowNumDeer){
			tempDeer += 3;
			//printf("Too much food for deer to eat, they reproduced extra 3 deer this month\n");
		}
		
		if(NowMonth >= 3 && NowMonth <= 5 && NowNumDeer > 1){
			tempDeer += -2;
			//printf("Hunting season, 1 more deer dead this month\n");
		}
		
		tempDeer -= NowNumWolf * ONE_WOLF_EATS_PER_MONTH;
		
		
		// DoneComputing barrier:
#pragma omp barrier
		NowNumDeer += tempDeer;
		NowHeight += tempGrow;
		NowNumWolf += tempWolf;
		
		if(NowNumDeer < 0){
			NowNumDeer = 0;
		}
		if(NowNumWolf < 0){
			NowNumWolf = 0;
		}
		// DoneAssigning barrier:
#pragma omp barrier
		
		// DonePrinting barrier:
#pragma omp barrier
		
	}
}



int main(){
	
	// starting date and time:
	NowMonth =    0;
	NowYear  = 2017;
	
	// starting state (feel free to change this if you want):
	NowNumDeer = 1;
	NowHeight =  1.;
	NowNumWolf = 0;
	
	omp_set_num_threads( 4 );
	
#pragma omp parallel sections
	{
#pragma omp section
		{
			GrainDeer( );
		}
		
#pragma omp section
		{
			Grain( );
		}
		
#pragma omp section
		{
			Watcher( );
		}
		
#pragma omp section
		{
			MyAgent( );	// your own
		}
	}       // implied barrier -- all functions must return in order
	// to allow any of them to get past here
	return 0;
}

