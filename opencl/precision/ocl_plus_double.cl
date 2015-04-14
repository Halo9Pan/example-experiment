#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#define TIMES 1024 * 1024 * 128

__kernel void plus_1d(
	__global double* input,
	__global double* output,
	const unsigned int count)
  {
	unsigned int global_index = get_global_id(0);
	if(global_index < count)
	  {
		double s = output[global_index];
		double a = input[global_index];
		for(long i = 0; i < TIMES; i++)
		  {
			s = s + a;
		  }
		output[global_index] = s;
	  }
  }
;
