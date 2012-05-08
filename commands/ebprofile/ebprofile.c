/* Blah
 * in MINIX 3. 
 */

// Remember to handle SIGKILL, clean up and tell kernel to stop profiling etc

#include <minix/ebprofile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if EBPROFILE

int outfile = 0;

_PROTOTYPE(int start, (void));
_PROTOTYPE(int stop, (void));
_PROTOTYPE(void help, (void));
_PROTOTYPE(int handle_args, (int argc, char *argv[]));

/* Starts event-based profiling. */
int
main (int argc, char *argv[])
{
  int action;

  /* bsd conformance */
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
    default:
      printf ("Incorrect arguments.\n");
      help ();
    }
}

/* Initializes buffers and starts profiling. */
int
start ()
{
  ebp_buffers *buffers;
  kcall_sample *consumer_buffer = (kcall_sample *) malloc(sizeof(kcall_sample[BUFFER_SIZE]));

  /* Allocates buffers and start profiling */
  buffers = ebp_start(0xFFF); // test bitmap

  /* Loop consumer, read buffers and write to file or socket */
  int i, j;
  j = 0;
  while (j != 1000)
  {
	j++;
	if (!ebp_get(consumer_buffer))
		continue;
	for (i=0; i<= BUFFER_SIZE; i++)
	{
		printf("m_type = %d, kcall = %d, p_nr = %d\n", ((kcall_sample *)consumer_buffer)[i]);
	}
  }
  return 0;
}

/* Stops event-based profiling in MINIX. */
// TODO pkill process, if exists. probably needs to report back to user what happened
int
stop () {
  ebp_stop();
  //sigaction(SIGKILL); // Read docs, noobz
  return 0;
}

/* Prints help. */
void
help ()
{
  printf ("Event-based profiling:\n");
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


#endif
