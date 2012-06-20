/* This is the example consumer commandline 
 * tool for event based profiling
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

//sigaction(SIGKILL callback = stop() ); // this would be nice, so we can clean up

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
  int i;

  printf("Consumption starting.\n");

  /* Allocates buffers and start profiling */
  buffers = ebp_start(0xFFF); // test bitmap, profiles everything

  /* initialize whatever we write to, socket, file or stdout */

  while (1)
  {
	reached = ebp_get(&consumer_buffer);
        if (reached == 0) continue;
        if (reached >= BUFFER_SIZE) 
        {
          printf("We are losing data! Not consuming fast enough. Lost %d samples.\n",reached-BUFFER_SIZE+1);
          reached = BUFFER_SIZE-1;
        }

	for (i=0; i< reached; i++)
	{
           /* Here we would write to a file or a socket or stdout */
          if (((consumer_buffer.sample)[i]).m_type == 5633) // use probe
          {
     	    printf("%s %s\n", 
     	      ((consumer_buffer.sample)[i]).typetext,
     	      ((consumer_buffer.sample)[i]).datatext);
     	    continue; 
     	  }

     	  printf("m_type = %d, field = %d, payload = %d, type = '%s', data = '%s'\n", 
     	    ((consumer_buffer.sample)[i]).m_type,
     	    ((consumer_buffer.sample)[i]).field,
     	    ((consumer_buffer.sample)[i]).payload,
     	    ((consumer_buffer.sample)[i]).typetext,
     	    ((consumer_buffer.sample)[i]).datatext);
     	   
	}
  }
  return 0;
}

/* Stops event-based profiling in MINIX. */
int
stop () {
  printf("Stopping profiling server.\n");
  ebp_stop();
  return 0;
}

/* Prints help. */
void
help ()
{
  printf ("Event-based profiling:\n");
  printf ("  |  ebprofile start \n");
  printf ("  |  ebprofile stop \n");
  printf ("  |  ebprofile [-h] \n");
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

