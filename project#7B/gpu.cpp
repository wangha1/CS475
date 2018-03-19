// 1. Program header

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <omp.h>

#include "CL/cl.h"
#include "CL/cl_platform.h"


#define NUM_ELEMENTS		2*32768

#define SIZE	32768

#ifndef LOCAL_SIZE
#define	LOCAL_SIZE		64
#endif

#define	NUM_WORK_GROUPS		NUM_ELEMENTS/LOCAL_SIZE

#define NUMTRY	50

const char *			CL_FILE_NAME = { "auto.cl" };
const float			TOL = 0.0001f;

void				Wait( cl_command_queue );
int				LookAtTheBits( float );


int
main( int argc, char *argv[ ] )
{
	// see if we can even open the opencl kernel program
	// (no point going on if we can't):
	
	FILE *fp;
	FILE *fp1;
#ifdef WIN32
	errno_t err = fopen_s( &fp, CL_FILE_NAME, "r" );
	if( err != 0 )
#else
		fp = fopen( CL_FILE_NAME, "r" );
	if( fp == NULL )
#endif
	{
		fprintf( stderr, "Cannot open OpenCL source file '%s'\n", CL_FILE_NAME );
		return 1;
	}
	
	cl_int status;		// returned status from opencl calls
	// test against CL_SUCCESS
	
	// get the platform id:
	
	cl_platform_id platform;
	status = clGetPlatformIDs( 1, &platform, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clGetPlatformIDs failed (2)\n" );
	
	// get the device id:
	
	cl_device_id device;
	status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clGetDeviceIDs failed (2)\n" );
	
	// 2. allocate the host memory buffers:
	
	int     Size;
	
	
	fp1 = fopen( "signal.txt", "r" );
	if( fp1 == NULL ){
		fprintf( stderr, "Cannot open file 'signal.txt'\n" );
		exit( 1 );
	}
	fscanf( fp1, "%d", &Size );
	Size = SIZE;
	float *hArray = new float[2 * Size];
	float  *hSums = new float[1 * Size];
	
	// fill the host memory buffers:
	for( int i = 0; i < Size; i++ ){
		fscanf( fp1, "%f", &hArray[i] );
		//printf("%d, %f\n",i, hArray[i]);
		hArray[i+Size] = hArray[i];		// duplicate the array
	}
	fclose( fp1 );
	//printf("I Am Here\n");
	
	// fill the host memory buffers:
	
	
	// 3. create an opencl context:
	
	cl_context context = clCreateContext( NULL, 1, &device, NULL, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateContext failed\n" );
	
	// 4. create an opencl command queue:
	
	cl_command_queue cmdQueue = clCreateCommandQueue( context, device, 0, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateCommandQueue failed\n" );
	
	// 5. allocate the device memory buffers:
	
	cl_mem dArray = clCreateBuffer( context, CL_MEM_READ_ONLY,  2*Size*sizeof(cl_float), NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (1)\n" );
	
	cl_mem dSums  = clCreateBuffer( context, CL_MEM_WRITE_ONLY, 1*Size*sizeof(cl_float), NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateBuffer failed (2)\n" );
	
	// 6. enqueue the 2 commands to write the data from the host buffers to the device buffers:
	
	status = clEnqueueWriteBuffer( cmdQueue, dArray, CL_FALSE, 0, 2*Size*sizeof(cl_float), hArray, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueWriteBuffer failed (1)\n" );
	
	status = clEnqueueWriteBuffer( cmdQueue, dSums, CL_FALSE, 0, 1*Size*sizeof(cl_float), hSums, 0, NULL, NULL );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clEnqueueWriteBuffer failed (2)\n" );
	
	Wait( cmdQueue );

	// 7. read the kernel code from a file:

	fseek( fp, 0, SEEK_END );
	size_t fileSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	char *clProgramText = new char[ fileSize+1 ];		// leave room for '\0'
	size_t n = fread( clProgramText, 1, fileSize, fp );
	clProgramText[fileSize] = '\0';
	fclose( fp );
	if( n != fileSize )
		fprintf( stderr, "Expected to read %d bytes read from '%s' -- actually read %d.\n", fileSize, CL_FILE_NAME, n );
	
	// create the text for the kernel program:
	
	char *strings[1];
	strings[0] = clProgramText;
	cl_program program = clCreateProgramWithSource( context, 1, (const char **)strings, NULL, &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateProgramWithSource failed\n" );
	delete [ ] clProgramText;
	
	// 8. compile and link the kernel code:
	
	char *options = { "" };
	status = clBuildProgram( program, 1, &device, options, NULL, NULL );
	if( status != CL_SUCCESS )
	{
		size_t size;
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size );
		cl_char *log = new cl_char[ size ];
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, size, log, NULL );
		fprintf( stderr, "clBuildProgram failed:\n%s\n", log );
		delete [ ] log;
	}
	
	// 9. create the kernel object:
	
	cl_kernel kernel = clCreateKernel( program, "AutoCorrelate", &status );
	if( status != CL_SUCCESS )
		fprintf( stderr, "clCreateKernel failed\n" );
	
	
	double maxMegaMults = 0.;
	double sumMegaMults = 0.;
	
	for(int i = 0; i < NUMTRY; i++){
		// 10. setup the arguments to the kernel object:
		
		status = clSetKernelArg( kernel, 0, sizeof(cl_mem), &dArray );
		if( status != CL_SUCCESS )
			fprintf( stderr, "clSetKernelArg failed (1)\n" );
		
		status = clSetKernelArg( kernel, 1, sizeof(cl_mem), &dSums  );
		if( status != CL_SUCCESS )
			fprintf( stderr, "clSetKernelArg failed (2)\n" );
		
		
		// 11. enqueue the kernel object for execution:
		
		size_t globalWorkSize[3] = { Size,         1, 1 };
		size_t localWorkSize[3]  = { LOCAL_SIZE,   1, 1 };
		
		
		Wait( cmdQueue );
		double time0 = omp_get_wtime( );
		
		time0 = omp_get_wtime( );
		
		status = clEnqueueNDRangeKernel( cmdQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL );
		if( status != CL_SUCCESS )
			fprintf( stderr, "clEnqueueNDRangeKernel failed: %d\n", status );
		
		Wait( cmdQueue );
		double time1 = omp_get_wtime( );
		
		// 12. read the results buffer back from the device to the host:
		
		status = clEnqueueReadBuffer( cmdQueue, dSums, CL_TRUE, 0, 1*Size*sizeof(cl_float), hSums, 0, NULL, NULL );
		if( status != CL_SUCCESS )
			fprintf( stderr, "clEnqueueReadBuffer failed\n" );
		
	
		double megaMults = (double)(Size * Size)/(time1-time0)/1000000.;
		sumMegaMults += megaMults;
		if( megaMults > maxMegaMults )
			maxMegaMults = megaMults;
		
	#ifdef WIN32
		Sleep( 2000 );
	#endif
	}
	double avgMegaMults = sumMegaMults/NUMTRY;
	printf( "   Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
	printf( "Average Performance = %8.2lf MegaMults/Sec\n", avgMegaMults );
	
	/*for (int i = 1; i<512; i++){
		printf("%f\n", hSums[i]);
	}*/
	
	
	
	// 13. clean everything up:
	
	clReleaseKernel(        kernel   );
	clReleaseProgram(       program  );
	clReleaseCommandQueue(  cmdQueue );
	clReleaseMemObject(     dSums );
	clReleaseMemObject(     dArray  );
	
	delete [ ] hSums;
	delete [ ] hArray;
	
	return 0;
}


int
LookAtTheBits( float fp )
{
	int *ip = (int *)&fp;
	return *ip;
}


// wait until all queued tasks have taken place:

void
Wait( cl_command_queue queue )
{
	cl_event wait;
	cl_int      status;
	
	status = clEnqueueMarker( queue, &wait );
	if( status != CL_SUCCESS )
		fprintf( stderr, "Wait: clEnqueueMarker failed\n" );
	
	status = clWaitForEvents( 1, &wait );
	if( status != CL_SUCCESS )
		fprintf( stderr, "Wait: clWaitForEvents failed\n" );
}
