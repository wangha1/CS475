from os import system
import math

size = 3;

while(size <= 9):
	size1 = 2 ** size;
	command = "g++ -DLOCAL_SIZE=" + str(size1) + " -o first first.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp";
	system(command);
	cmd = ("./first >> result1.csv");
	system(cmd);
	size += 1

cmd = ("cat result1.csv");
system(cmd);

