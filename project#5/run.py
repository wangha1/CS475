from os import system

size = 1000;

while(size <= 32000000):
	cmd = ("g++ -DARRAY_SIZE={} -o project project#5_mul_red.cpp simd.p5.cpp -lm -fopenmp").format(size);
	system(cmd);
	cmd = ("./project >> result1");
	system(cmd);
	cmd = ("cat result1");
	system(cmd);
##size = size * 2;
	if(size >= 10000000):
		size += 10000000
	elif(size >= 1000000):
		size += 1000000
	elif(size >= 100000):
		size += 100000
	elif(size >= 10000):
		size += 10000
	else:
		size += 1000

cmd = ("cat result1");
system(cmd);
