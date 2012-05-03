/* Blah
 * in MINIX 3. 
 */

// Remember to handle SIGKILL, clean up and tell kernel to stop profiling etc

#include "ebprof.h"
#include <sys/signal.h>
#include <signal.h>

#if EBPROFILE

#define HELP  0
#define START 1
#define STOP  2

// TODO fix buffer size
int buf_size = 0;
int outfile = 0;

extern int *fir_buf;
extern int *sec_buf;

int start (void);
int stop (void);
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

/* Initializes buffers and starts profiling. */
int
start ()
{
  fir_buf = alloc_buffers ();
  sec_buf = alloc_buffers ();

  /* Start profiling in kernel */
  do_ebprofile(fir_buf, sec_buf);  

  // Loop consumer, read buffers and write to file or socket (SUCK IT!) 
  while (1)
  {

  }
  return 0;
}

/* Stops event-based profiling in MINIX. */
// TODO pkill process, if exists. probably needs to report back to user what happened
int
stop ()
{

  sigaction(SIGKILL); // Read docs, noobz
  
  return 0;
}

/* Prints help. */
void
help ()
{
  printf ("Event-based profiling:\n")
  printf ("  ebprofile start [-f filename | -n ip:port] [-d]\n");
  printf ("  ebprofile stop\n");
  printf ("Use ebprofalyze.pl to analyze output file.\n");
  return;
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
}
      return HELP;
    }
}

#endif
