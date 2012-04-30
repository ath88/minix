/* This file implements event-based profiling
 * in MINIX 3. 
*/

#include "ebprof.h"

#if EBPROFILE

int initialize (void);
int collect (char* kcall, int size);
int alloc_buffers (void);

int buf_size = 0;
extern int *pri_buf;
extern int *sec_buf;

/* Initializes datastructures used for profiling */
int
initialize()
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

/* Allocates memory for double buffering */
int
alloc_buffers ()
{
  pri_buf = malloc (buf_size);
  sec_buf = malloc (buf_size);

  if (pri_buf == 0 || sec_buf == 0)
    {
      printf
	("Could not allocate buffers. Disabling event-based profiling.\n")
	return 1;
    }
  else
    {
      memset (pri_buf, '\0', buf_size);
      memset (sec_buf, '\0', buf_size);
    }
	return 0;
}

#endif /* EBPROFILE */
