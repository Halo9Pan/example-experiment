/*
 * devices.c
 *
 *  Created on: Apr 14, 2015
 *      Author: Halo9Pan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <CL/opencl.h>

#define MAX_PLATFORM_NUMBER 4
#define MAX_DEVICE_NUMBER 4

#define CL_CHECK(s, err) if (err != CL_SUCCESS)\
	{\
	  printf ("%s %d\n", s, err);\
	  return NULL;\
	}\

cl_device_id
retrieve_device (char* platform_vender)
{
  cl_int err;
  cl_device_id device_id = NULL;
  cl_platform_id platform_ids[MAX_PLATFORM_NUMBER];

  cl_uint ret_num_devices = 0;
  cl_uint ret_num_platforms = 0;

  err = clGetPlatformIDs (MAX_PLATFORM_NUMBER, platform_ids, &ret_num_platforms);
  CL_CHECK("Error: Failed to create a platform!", err)
  printf("There are %d platforms totally.\n", ret_num_platforms);

  int i;
  for (i = 0; i < ret_num_platforms; ++i) {
	  cl_platform_id platform_id = platform_ids[i];
	  char platform_name[1024];
	  err = clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 1024, platform_name, NULL);
	  if(strcmp(platform_vender, platform_name) < 0)
		{
		  err = clGetDeviceIDs (platform_id, CL_DEVICE_TYPE_ALL, MAX_DEVICE_NUMBER, &device_id, &ret_num_devices);
		  CL_CHECK("Error: Failed to create a device group!", err)
		  char device_name[1024];
		  err = clGetDeviceInfo (device_id, CL_DEVICE_NAME, 1024, device_name, NULL);
		  printf("Get device \"%s\" in platform \"%s\".\n", device_name, platform_name);
		  return device_id;
		}
  }

  return device_id;
}
