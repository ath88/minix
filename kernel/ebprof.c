/* This file implements event-based profiling
 * in MINIX 3. 
 */

#include "ebprof.h"

#if EBPROFILE

#define HELP  0
#define START 1
#define STOP  2
#define GET   3

int buf_size = 0;
int outfile = 0;

extern int *pri_buf;
extern int *sec_buf;

int start (void);
int stop (void);
int get (void);
int help (void);
int collect (void);
int handle_args (int argc, char *argv[]);
int alloc_buffers (void);

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
start ()
{
  pri_buf = alloc_buffers ();
  sec_buf = alloc_buffers ();

  /* Start profiling in kernel */
  return 0;
}

/* Stops event-based profiling in MINIX. */
int
stop ()
{
  return 0;
}

/* Print current profiling information. */
int
get ()
{
  return 0;
}

/* Prints help. */
int
help ()
{
  printf ("Event-based profiling:\n")
    printf ("  ebprofile start [-o outfile]\n");
  printf ("  ebprofile stop\n");
  printf ("  ebprofile get [-o outfile]\n");
  printf ("Use ebprofalyze.pl to analyze output file.\n");
  return 0;
}

/* Write profiling information to buffer */
int
collect (message * m_user, struct proc *caller)
{
  return 0;
}

/* Handle arguments for ebprofile. */
int
handle_args (int argc, char *argv[])
{
  while (--argc)
    {
      ++argv;
      if (strcmp (*argv, "-h") == 0 || strcmp (*argv, "help") == 0 ||
	  strcmp (*argv, "--help") == 0)
	{
	  return HELP;
	}
      else if (strcmp (*argv, "start") == 0)
	{
	  return START;
	}
      else if (strcmp (*argv, "stop") == 0)
	{
	  return STOP;
	}
      else if (strcmp (*argv, "get") == 0)
	{
	  return GET;
	}
      return -1;
    }
}

#endif
