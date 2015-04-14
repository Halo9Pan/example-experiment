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

#define CL_CHECK(s, err) if (err != CL_SUCCESS)\
	{\
	  printf ("%s %d\n", s, err);\
	  return EXIT_FAILURE;\
	}\

char*
read_file (char* file_name);

cl_device_id
retrieve_device (char* device_name);

// Use a static data size for simplicity
#define DATA_SIZE (256)

typedef cl_double cl_unit;
#define CL_FILE "ocl_plus_double.cl"

int
ocl_plus_1d ()
{
  printf ("DATA_SIZE: %d\n", DATA_SIZE);
  int err;                            // error code returned from api calls

  cl_unit data[DATA_SIZE];            // original data set given to device
  cl_unit results[DATA_SIZE];         // results returned from device
  unsigned int correct;               // number of correct results returned

  size_t global;          // global domain size for our calculation
  size_t local;                       // local domain size for our calculation

  cl_device_id device_id;             // compute device id
  cl_context context;                 // compute context
  cl_command_queue commands;          // compute command queue
  cl_program program;                 // compute program
  cl_kernel kernel;                   // compute kernel

  cl_mem input;                       // device memory used for the input array
  cl_mem output;                      // device memory used for the output array

  clock_t cl_init, cl_finished, cl_start, cl_end, start, end;

  // Fill our data set with random float values
  int i = 0;
  unsigned int count = DATA_SIZE;
  for (i = 0; i < count; i++)
	{
	  data[i] = (cl_unit)(rand () * (ULLONG_MAX / RAND_MAX));
	}

  // Connect to a compute device
  cl_init = clock ();
  device_id = retrieve_device("NVIDIA");

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
  char *kernel_source_int = read_file(CL_FILE);
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
  kernel = clCreateKernel (program, "plus_1d", &err);
  if (!kernel || err != CL_SUCCESS)
	{
	  printf ("Error: Failed to create compute kernel!\n");
	  exit (EXIT_FAILURE);
	}

  // Create the input and output arrays in device memory for our calculation
  input = clCreateBuffer (context, CL_MEM_READ_ONLY, sizeof(cl_unit) * count, NULL, NULL);
  output = clCreateBuffer (context, CL_MEM_WRITE_ONLY, sizeof(cl_unit) * count, NULL, NULL);
  if (!input || !output)
	{
	  printf ("Error: Failed to allocate device memory!\n");
	  exit (EXIT_FAILURE);
	}

  // Write our data set into the input array in device memory
  err = clEnqueueWriteBuffer (commands, input, CL_TRUE, 0, sizeof(cl_unit) * count, data, 0, NULL, NULL);
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

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
//err = clEnqueueNDRangeKernel (commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
  global = count;
  err = clEnqueueNDRangeKernel (commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
  printf("========================================\n");
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
  err = clEnqueueReadBuffer (commands, output, CL_TRUE, 0, sizeof(cl_unit) * count, results, 0, NULL, NULL);
  if (err != CL_SUCCESS)
	{
	  printf ("Error: Failed to read output array! %d\n", err);
	  exit (EXIT_FAILURE);
	}
  cl_finished = clock ();

  // Validate our results
  start = clock ();
  correct = 0;
  for (i = 0; i < count; i++)
	{
//	  printf("%d + %d = %d\n", data[i], data[i], results[i]);
	  if (results[i] == (data[i]) + (data[i]))
		correct++;
	}
  end = clock ();


  // Print a brief summary detailing the results
  printf ("Computed '%d/%d' correct values!\n", correct, count);
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

  return EXIT_SUCCESS;
}
