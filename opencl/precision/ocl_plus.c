#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/opencl.h>

extern int
c_plus_example ();

char*
read_file (char* file_name);

// Use a static data size for simplicity
#define DATA_SIZE (512)

const char *KernelSourceLong = "\n"
	"__kernel void square(                                                  \n"
	"   __global long* input,                                                \n"
	"   __global long* output,                                               \n"
	"   const unsigned int count)                                           \n"
	"{                                                                      \n"
	"   int i = get_global_id(0);                                           \n"
	"   if(i < count)                                                       \n"
	"       output[i] = (input[i]) + (input[i]);                            \n"
	"}                                                                      \n"
	"\n";

////////////////////////////////////////////////////////////////////////////////

int
ocl_plus ()
{
  printf ("DATA_SIZE： %d\n", DATA_SIZE);
  int err;                            // error code returned from api calls

  int int_data[DATA_SIZE][DATA_SIZE];            // original data set given to device
  int int_results[DATA_SIZE][DATA_SIZE];         // results returned from device
//  long long long_data[DATA_SIZE];
//  long long long_result[DATA_SIZE];
  unsigned int correct;               // number of correct results returned

  size_t global[] = {DATA_SIZE, DATA_SIZE};;                      // global domain size for our calculation
  size_t local[2];                       // local domain size for our calculation

  cl_device_id device_id;             // compute device id
  cl_context context;                 // compute context
  cl_command_queue commands;          // compute command queue
  cl_program program;                 // compute program
  cl_kernel kernel;                   // compute kernel
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices = 0;
  cl_uint ret_num_platforms = 0;

  cl_mem input;                       // device memory used for the input array
  cl_mem output;                      // device memory used for the output array

  clock_t cl_init, cl_finished, cl_start, cl_end, start, end;
  ;
  // Fill our data set with random float values
  int i,j = 0;
  unsigned int count = DATA_SIZE;
  for (i = 0; i < count; i++)
	{
	  for (j = 0; j < count; j++) {
		  int_data[i][j] = rand () * (UINT_MAX / RAND_MAX);
	  }
//	  long_data[i] = rand () * (ULLONG_MAX / RAND_MAX);
	}

  // Connect to a compute device
  cl_init = clock ();
  int gpu = 1;
  err = clGetPlatformIDs (4, &platform_id, &ret_num_platforms);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to create a platform! %d\n", err);
	  return EXIT_FAILURE;
	}
  printf ("Platforms number: %d\n", ret_num_platforms);
//printf ("Platform IDs: %s\n", platform_id);
  err = clGetDeviceIDs (platform_id, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, &ret_num_devices);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to create a device group! %d\n", err);
	  return EXIT_FAILURE;
	}
  char buffer[1024];
  err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to retrieve device vender! %d\n", err);
	  return EXIT_FAILURE;
	}
  else
	{
	  printf ("CL_DEVICE_VENDOR: %s\n", buffer);
	}

  // Create a compute context
  context = clCreateContext (0, 1, &device_id, NULL, NULL, &err);
  if (!context)
	{
	  printf ("Error: Failed to create a compute context!\n");
	  return EXIT_FAILURE;
	}

  // Create a command commands
  commands = clCreateCommandQueue (context, device_id, 0, &err);
  if (!commands)
	{
	  printf ("Error: Failed to create a command commands!\n");
	  return EXIT_FAILURE;
	}

  // Simple compute kernel which computes the square of an input array
  char *kernel_source_int = read_file("ocl_plus_int.cl");
  printf("The contents of file are :\n%s\n", kernel_source_int);

  // Create the compute program from the source buffer
  program = clCreateProgramWithSource (context, 1, (const char **) &kernel_source_int, NULL, &err);
  if (!program)
	{
	  printf ("Error: Failed to create compute program!\n");
	  return EXIT_FAILURE;
	}

  // Build the program executable
  err = clBuildProgram (program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
	{
	  size_t len;
	  char buffer[2048];

	  printf ("Error: Failed to build program executable!\n");
	  clGetProgramBuildInfo (program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
	  printf ("%s\n", buffer);
	  exit (EXIT_FAILURE);
	}

  // Create the compute kernel in the program we wish to run
  kernel = clCreateKernel (program, "square", &err);
  if (!kernel || err != CL_SUCCESS)
	{
	  printf ("Error: Failed to create compute kernel!\n");
	  exit (EXIT_FAILURE);
	}

  // Create the input and output arrays in device memory for our calculation
  input = clCreateBuffer (context, CL_MEM_READ_ONLY, sizeof(int) * count * count, NULL, NULL);
  output = clCreateBuffer (context, CL_MEM_WRITE_ONLY, sizeof(int) * count * count, NULL, NULL);
  if (!input || !output)
	{
	  printf ("Error: Failed to allocate device memory!\n");
	  exit (EXIT_FAILURE);
	}

  // Write our data set into the input array in device memory
  err = clEnqueueWriteBuffer (commands, input, CL_TRUE, 0, sizeof(int) * count * count, int_data, 0, NULL, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to write to source array!\n");
	  exit (EXIT_FAILURE);
	}

  // Set the arguments to our compute kernel
  err = 0;
  err = clSetKernelArg (kernel, 0, sizeof(cl_mem), &input);
  err |= clSetKernelArg (kernel, 1, sizeof(cl_mem), &output);
  err |= clSetKernelArg (kernel, 2, sizeof(unsigned int), &count);
  err |= clSetKernelArg (kernel, 3, sizeof(cl_int) * count, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to set kernel arguments! %d\n", err);
	  exit (EXIT_FAILURE);
	}

  // Get the maximum work group size for executing the kernel on the device
  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to retrieve kernel work group info! %d\n", err);
	  exit (EXIT_FAILURE);
	}
//  size_t size;
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_WORK_GROUP_SIZE: %I64u\n", size);
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_COMPILE_WORK_GROUP_SIZE: %I64u\n", size);
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE: %I64u\n", size);
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_GLOBAL_WORK_SIZE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_GLOBAL_WORK_SIZE: %I64u\n", size);
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_LOCAL_MEM_SIZE: %I64u\n", size);
//  err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(size), &size, NULL);
//  printf("CL_KERNEL_PRIVATE_MEM_SIZE: %I64u\n", size);

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
//  err = clEnqueueNDRangeKernel (commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
  err = clEnqueueNDRangeKernel (commands, kernel, 2, NULL, global, NULL, 0, NULL, NULL);
  if (err)
	{
	  printf ("Error: Failed to execute kernel!\n");
	  return EXIT_FAILURE;
	}

  // Wait for the command commands to get serviced before reading back results
  cl_start = clock ();
  clFinish (commands);
  cl_end = clock ();

  // Read back the results from the device to verify the output
  err = clEnqueueReadBuffer (commands, output, CL_TRUE, 0, sizeof(int) * count * count, int_results, 0, NULL, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to read output array! %d\n", err);
	  exit (EXIT_FAILURE);
	}
  cl_finished = clock ();

  // Validate our results
  //
  start = clock ();
  correct = 0;
  for (i = 0; i < count; i++)
	{
	  for (j = 0; j < count; j++) {
		if (int_results[i][j] == (int_data[i][j]) + (int_data[i][j]))
		  correct++;
	  }
	}
  end = clock ();

/*
  for (i = 0; i < count; i++)
	{
	  printf("%d + %d = %d\n", (data[i] >> 16), (data[i] >> 16), results[i]);
	}
*/

  // Print a brief summary detailing the results
  printf ("Computed '%d/%d' correct values!\n", correct, count * count);
  printf ("Time required for OpenCL execution: %d - %d = %d\n", (int) cl_finished, (int) cl_init, (int) (cl_finished - cl_init));
  printf ("Time required for OpenCL computing: %d - %d = %d\n", (int) cl_end, (int) cl_start, (int) (cl_end - cl_start));
  printf ("Time required for normal execution: %d - %d = %d\n", (int) end, (int) start, (int) (end - start));

  // Shutdown and cleanup
  clReleaseMemObject (input);
  clReleaseMemObject (output);
  clReleaseProgram (program);
  clReleaseKernel (kernel);
  clReleaseCommandQueue (commands);
  clReleaseContext (context);

//  c_plus_example ();
  return 0;
}
