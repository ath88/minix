/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include "ebp.h"

#if EBPROFILE

extern int *fir_buf;
extern int *sec_buf;

int start (void);
int stop (void);
int get (void);
int collect (void);
int *alloc_buffers (void);

/* Initializes datastructures used for profiling. */
int
start ()
{
  fir_buf = alloc_buffers();
  sec_buf = alloc_buffers();

  sys_ebprof(caller, msg);

  return 0;
}

/*  Stops profiling and deallocates memory. */
void
stop ()
{
  disable_ebprof();
  return;
}


/* Write current profiling information to buffer. */
int
get (void *buffer)
{
  return 0;
}

/* Write profiling information to buffer */
int
collect (message * m_user, struct proc *caller)
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

/* Returns pointer to active buffer */
int*
active_buffer()
{

}

/* Returns pointer to inactive buffer */
int*
inactive_buffer()
{

}


/* Returns whether or not profiling is enabled */
int
ebprofiling()
{
	return ebp_bm & 1;
}

/* Enable event-based profiling */
int
enable_ebprof()
{
	return ebp_bm |= 1;
}

/* Disable event-based profiling */
int
disable_ebprof()
{
	return ebp_bm &= 0;
}

#endif /* EBPROFILE */
