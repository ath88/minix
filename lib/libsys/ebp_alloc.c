/* This file implements memory allocation for
 * event-based profiling.
 */

#include <ebprof.c>

#if EBPROFILE

int *alloc_buffers (void);

/* Allocates memory for double buffering */
int*
alloc_buffers ()
{
  buffer = malloc (sizeof (kcall_sample[1024]));

  if (buffer == 0)
    {
      printf
	("Could not allocate buffers. Disabling event-based profiling.\n")
	return 1;
    }
  else
    {
      memset (buffer, '\0', sizeof (kcall_sample[1024]));
    }
  return buffer;
}

#endif /* EBPROFILE */
