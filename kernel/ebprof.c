/* This file implements event-based profiling
 * in MINIX 3. 
 */

#include "ebprof.h"

#if EBPROFILE

int start (void);
int collect (void);
int alloc_buffers (void);

int buf_size = 0;
extern int *pri_buf;
extern int *sec_buf;

/* Initializes datastructures used for profiling */
int
start()
{
	alloc_buffers();
	return 0;
}

/* Write profiling information to buffer */
int
collect(char* kcall, int size)
{
	return 0;
}

#endif /* EBPROFILE */
