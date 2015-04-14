#define TIMES 1024 * 1024 * 16

__kernel void plus_1d(
	__global float* input,
	__global float* output,
	const unsigned int count)
  {
	unsigned int global_index = get_global_id(0);
	if(global_index < count)
	  {
		float s = output[global_index];
		float a = input[global_index];
		for(long i = 0; i < TIMES; i++)
		  {
			s = s + a;
		  }
		output[global_index] = s;
	  }
  }
;
