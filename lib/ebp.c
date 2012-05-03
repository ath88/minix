/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include "ebp.h"

#if EBPROFILE

int start (void);
int stop (void);
int get (void);
int collect (void);
int *alloc_buffers (void);

/* Initializes datastructures used for profiling. */
int
ebp_start ()
{
  epb_buffers *buffers;
  
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();

  // Create bitmap here
  
  /* do syscall */ 
  sys_ebprof(buffers->first, buffers->second, epb_bm);

  return buffers;
}

/*  Stops profiling and deallocates memory. */
void
stop ()
{
  sys_ebprof(NULL, NULL, 0b0); // Check binary
  return;
}


/* Write current profiling information to buffer. */
int
ebp_get (void *buffer)
{
  return 0;
}

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
