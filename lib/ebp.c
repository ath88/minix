/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include "ebprof.h"

#if EBPROFILE

int buf_size = 0;
int outfile = 0;

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
  do_ebprofile();

  return 0;
}

/*  Stops profiling and deallocates memory. */
int
stop ()
{
  return 0;
}


/* Write current profiling information to buffer. */
int
get ()
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

#endif /* EBPROFILE */
