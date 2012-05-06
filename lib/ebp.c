/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include "ebp.h"

#if EBPROFILE

extern unsigned int switch_buffer;
int ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
int *alloc_buffers (void);
int *inactive_buffer;

/* Initializes datastructures used for profiling. */
int
ebp_start (int bitmap)
{
  epb_buffers *buffers;
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();

  /* Set profiling flag */
  bitmap &= 0x1;
 
  /* do syscall */ 
  sys_ebprof(buffers->first, buffers->second, bitmap);

  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  sys_ebprof(NULL, NULL, 0x0);
  return;
}


/* Write current profiling information to buffer. */
int
ebp_get (void *buffer)
{  
  // Try to get lock, if it gets
  
  buffer = inactive_buffer;
  //Lock stuff
  return 0;
}

/* Allocates memory for double buffering */
int*
alloc_buffers (void)
{
  buffer = malloc (sizeof (kcall_sample[BUFFER_SIZE]));

  if (buffer == 0)
    {
      printf
	("Could not allocate buffers. Disabling event-based profiling.\n")
	return 1;
    }
  else
    {
      memset (buffer, '\0', sizeof (kcall_sample[BUFFER_SIZE]));
    }
  return buffer;
}

#endif /* EBPROFILE */
