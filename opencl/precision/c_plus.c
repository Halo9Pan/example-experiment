/*
 ============================================================================
 Name        : samples-c-hello-cdt.c
 Author      : Halo9Pan
 Version     :
 Copyright   : CopyLeft
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

int c_plus_example(void) {
	printf("!!!Hello %s!!!\n", "World"); /* prints !!!Hello World!!! */
	uint32_t i = UINT_MAX;
	printf("The max unsigned integer number is %u[%x]\n", i, i);
	uint64_t l = ULLONG_MAX;
	printf("The max unsigned long number is %I64u[%I64x]\n", l, l);
	uint64_t l4 = l >> (4 * 8);
	printf("The long shift 4 right number is %I64u[%I64x]\n", l4, l4);
	uint64_t r = rand() * (ULLONG_MAX / RAND_MAX);
	printf("The random long number is %I64u[%I64x]\n", r, r);
	uint32_t rr = r >> (4 * 8);
	printf("The long random number right is %u[%x]\n", rr, rr);
	uint32_t rl = r;
	printf("The long random number left is %u[%x]\n", rl, rl);
	return EXIT_SUCCESS;
}
