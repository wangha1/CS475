from os import system

size = 1;

while(size <= 8 * 1024):
	size1 = size * 1024;
	command = "g++ -DNUM_ELEMENTS=" + str(size1) + " -o first first.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp";
	system(command);
	cmd = ("./first >> result.csv");
	system(cmd);
	size *= 2;

cmd = ("cat result.csv");
system(cmd);

