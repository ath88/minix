/* Blah
 * in MINIX 3. 
 */

#include <minix/ebprofile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int c;
int daemonize;
int fd;
char *cval;

int start (void);
int stop (void);
void help (void);
int handle_args (int argc, char *argv[]);


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
      break;
    case START:
      start();
      break;
    case STOP:
      stop ();
      break;
    }
  exit(0);
}

/* Initializes buffers and starts profiling. */
int
start ()
{
  ebp_buffers *buffers;
  ebp_sample_buffer consumer_buffer;
  int reached;

  printf("Starting..\n");
  /* Allocates buffers and start profiling */
  buffers = ebp_start(0xFFF); // test bitmap, profiles everything

  printf("buffers allocated, buffer1 = 0x%x, buffer2 = 0x%x relbuf = 0x%x2\n",buffers->first,buffers->second,buffers->relbuf);

  /* Loop consumer, read buffers and write to file or socket */
  int i;

  /* NEW FILE OR SOCKET MAGIC */
  while (1)
  {
//       	printf("first reached = %d\n", buffers->first->reached);
//       	printf("second reached = %d\n", buffers->second->reached);

	reached = ebp_get(&consumer_buffer);
        if (reached == 0) continue;

//        printf("soo, does this work?, reached = %d \n", reached);

	for (i=0; i< reached; i++)
	{
           /* Here we would write to a file or a socket or stdout */
        printf("soo, does this work?\n");
     	printf("m_type = %d, field = %d, payload = %d\n", 
     	  ((consumer_buffer.sample)[i]).m_type,
     	  ((consumer_buffer.sample)[i]).field,
     	  ((consumer_buffer.sample)[i]).payload);

	}
  }
  return 0;
}

/* Stops event-based profiling in MINIX. */
// TODO pkill process, if exists. probably needs to report back to user what happened
int
stop () {
  printf("Stopping");
  ebp_stop();
  //sigaction(SIGKILL); // Read docs, noobz
  return 0;
}

/* Prints help. */
void
help ()
{
  printf ("Event-based profiling:\n");
  printf ("  |  ebprofile start [-f filename | -n ip:port] [-d]\n");
  printf ("  |  ebprofile stop \n");
  printf ("  |  ebprofile [-h] \n");
  printf ("Use ebprofalyze.pl to analyze output file.\n");
  sleep(1);
  return;
}

/* Handle arguments for ebprofile. */
int
handle_args (int argc, char *argv[])
{
  int action;
  daemonize = 0;
  fd = STDOUT;

  if (argv[1] == NULL) return HELP;

  /* determine action */
  if (strcmp (argv[1], "start") == 0)
  {
       action = START;
  }
  else if (strcmp (argv[1], "stop") == 0)
  {     
       action = STOP;
  }
  else return HELP;

  /* parse options */
  while ((c = getopt(++argc, argv, "f:n:dh")) != -1)
    {
      switch(c)
      {
          case 'f':
                cval = optarg;
                fd = LFILE;
                break;   
          case 'n':
                cval = optarg;
                fd = NETW;
                break;
          case 'd':
                daemonize = 1;
                break;
          case 'h':
           	action = HELP;
                break;
          default:
                printf("Unknown option character.\n");
                action = HELP;
      }
    }
   return action;
}  

