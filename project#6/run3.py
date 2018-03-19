from os import system

size = 1;

while(size <= 8 * 1024):
	size1 = size * 1024;
	command = "g++ -DNUM_ELEMENTS=" + str(size1) + " -DLOCAL_SIZE=32 -o proj second.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp";
	system(command);
	cmd = ("./proj >> result_red.csv");
	system(cmd);
	command = "g++ -DNUM_ELEMENTS=" + str(size1) + " -DLOCAL_SIZE=64 -o proj second.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp";
	system(command);
	cmd = ("./proj >> result_red.csv");
	system(cmd);
	command = "g++ -DNUM_ELEMENTS=" + str(size1) + " -DLOCAL_SIZE=256 -o proj second.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp";
	system(command);
	cmd = ("./proj >> result_red.csv");
	system(cmd);

	size *= 2;

cmd = ("cat result_red.csv");
system(cmd);


