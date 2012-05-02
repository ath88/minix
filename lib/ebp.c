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

/* Starts event-based profiling. */
int
main (int argc, char *argv[])
{
  int action;

  setprogname (argv[0]);

  action = handle_args (argc, argv);
  switch (action)
    {
    case HELP:
      help ();
    case START:
      start ();
    case STOP:
      stop ();
    case GET:
      get ();
    default:
      printf ("Incorrect arguments.\n");
      help ();
    }
}

/* Initializes datastructures used for profiling. */
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
  buffer = malloc (sizeof (kcall_sample));

  if (buffer == 0)
    {
      printf
	("Could not allocate buffers. Disabling event-based profiling.\n")
	return 1;
    }
  else
    {
      memset (buffer, '\0', sizeof (kcall_sample));
    }
  return buffer;
}

#endif /* EBPROFILE */
