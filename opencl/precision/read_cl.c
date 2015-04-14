/*
 * read_cl.c
 *
 *  Created on: Apr 3, 2015
 *      Author: panhao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
read_file (char *file_name)
{
  FILE *fp;
  char *p;

  fp = fopen (file_name, "r"); // read mode

  if (fp == NULL)
	{
	  perror ("Error while opening the file.\n");
	  exit (EXIT_FAILURE);
	}
  int flen = 0;
  fseek (fp, 0L, SEEK_END);
  flen = ftell (fp);
  p = (char *) malloc (flen + 1);
  if (p == NULL)
	{
	  fclose (fp);
	  return NULL;
	}
  fseek (fp, 0L, SEEK_SET);
  fread (p, flen, 1, fp);
  p[flen] = 0;
  fclose (fp);
  return p;
}
