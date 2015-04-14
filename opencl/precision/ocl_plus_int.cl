#define ADD(s, x, y) s = x + y
#define ADD4(s, x, y) ADD(s, x, y); ADD(s, x, y); ADD(s, x, y); ADD(s, x, y);
#define ADD16(s, x, y) ADD4(s, x, y); ADD4(s, x, y); ADD4(s, x, y); ADD4(s, x, y);
#define ADD64(s, x, y) ADD16(s, x, y); ADD16(s, x, y); ADD16(s, x, y); ADD16(s, x, y);
#define ADD256(s, x, y) ADD64(s, x, y); ADD64(s, x, y); ADD64(s, x, y); ADD64(s, x, y);

#define TIMES 1024 * 1024 * 64

__kernel void square(
	__global int* input,
	__global int* output,
	const unsigned int count,
	__local int* buffer)
  {
	unsigned int global_index_0 = get_global_id(0);
	unsigned int global_index_1 = get_global_id(1);
	int local_index = get_local_id(0);
	int local_size = get_local_size(0);
	if(global_index_0 < count)
	  {
		int i = input[global_index_0];
//		printf("[%d][%d]:%d=%d\n", global_index_0, global_index_1, local_index, i);
		buffer[local_index] = input[global_index_0];
		for(int j = 0; j < count; j++)
		  {
//			printf("%d", input[global_index][j]);
		  }
	  }
//	for(int offset = 1; offset < get_local_size(0); offset++)
//	  {
//		output_buffer[local_index] = (input_buffer[local_index]) + (input_buffer[local_index]);
//	  }
  }
;

__kernel void plus_1d(
	__global int* input,
	__global int* output,
	const unsigned int count)
  {
	unsigned int global_index = get_global_id(0);
	if(global_index < count)
	  {
		for(int i = 0; i < TIMES; i++)
		  {
			output[global_index] = input[global_index] + input[global_index];
		  }
	  }
  }
;
