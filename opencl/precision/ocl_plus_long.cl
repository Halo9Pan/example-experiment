#define TIMES 1024 * 1024 * 64

__kernel void plus_1d(
	__global long* input,
	__global long* output,
	const unsigned int count)
  {
	unsigned int global_index = get_global_id(0);
	if(global_index < count)
	  {
		long s = output[global_index];
		long a = input[global_index];
		for(int i = 0; i < TIMES; i++)
		  {
			s = s + a;
		  }
		output[global_index] = s;
	  }
  }
;
