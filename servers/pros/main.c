/* Profiling Service. 
 * This service implements functions for intercepting messages to the kernel
 * and gather event-based profiling data such as which kernel calls are invoked.
 */

#include "inc.h"	/* include master header file */
#include <minix/endpoint.h>

/* Allocate space for the global variables. */
PRIVATE endpoint_t who_e;	/* caller's proc number */
PRIVATE int callnr;		/* system call number */

/* Declare some local functions. */
FORWARD _PROTOTYPE(void get_work, (message *m_ptr)                      );
FORWARD _PROTOTYPE(void reply, (endpoint_t whom, message *m_ptr)	);
FORWARD _PROTOTYPE(void write_buffer, (message *m_ptr)	);

/* SEF functions and variables. */
FORWARD _PROTOTYPE( void sef_local_startup, (void) );

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
PUBLIC int main(int argc, char **argv)
{
/* This is the main routine of this service. The main loop consists of 
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  message m;
  int result;                 

  /* SEF local startup. */
  env_setargs(argc, argv);
  sef_local_startup();

  /* Main loop - get work and do it, forever. */         
  while (TRUE) {              

      /* Wait for incoming message, sets 'callnr' and 'who'. */
      get_work(&m);

      if (is_notify(callnr)) { //What does this do?
          printf("PROS: warning, got illegal notify from: %d\n", m.m_source);
          result = EINVAL;
      }
   
  }
  return(OK);				/* shouldn't come here */
}


/*===========================================================================*
 *				write_buffer                                 *
 *===========================================================================*/
PRIVATE void write_buffer(
  message *m_ptr			/* message buffer */
)
{


}


/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
PRIVATE void sef_local_startup()
{
  /* Register init callbacks. */

  /* No live update support for now. */

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
PRIVATE void get_work(
  message *m_ptr			/* message buffer */
)
{
    int status = sef_receive(ANY, m_ptr);   /* blocks until message arrives */
    if (OK != status)
        panic("failed to receive message!: %d", status);
    who_e = m_ptr->m_source;        /* message arrived! set sender */
    callnr = m_ptr->m_type;       /* set function call number */
}

