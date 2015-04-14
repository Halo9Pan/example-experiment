/*
 * main.c
 *
 *  Created on: Apr 7, 2015
 *      Author: Halo9Pan
 */
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
ocl_plus_1d ();

int
main (int argc, char** argv)
{
  printf ("Starting...\n");
  //retrieve_device("NVIDIA");
  ocl_plus_1d ();
  return EXIT_SUCCESS;
}
