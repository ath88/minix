/* Profiling Service. 
 * This service implements functions for intercepting messages to the kernel
 * and gather event-based profiling data such as which kernel calls are invoked.
 */

#include "inc.h"	/* include master header file */
#include "glo.h"
#include <minix/endpoint.h>
#include <sys/shm.h>

/* Allocate space for the global variables. */
PRIVATE endpoint_t who_e;	/* caller's proc number */
PRIVATE int callnr;		/* system call number */

/* Declare some local functions. */
FORWARD _PROTOTYPE(void get_work, (message *m_ptr)                      );
FORWARD _PROTOTYPE(void reply, (endpoint_t whom, message *m_ptr)	);
FORWARD _PROTOTYPE(void write_buffer, (message *m_ptr)          	);
FORWARD _PROTOTYPE(void do_ctl, (message *m_ptr)	                );
FORWARD _PROTOTYPE(void attach_memory, (key_t key1, key_t key2)         );

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

      switch (callnr)
      {
              case PROS_CTL:
                 //Send reply
                 result = do_ctl(&m);
                 setreply(who_p, result);
                 sendreply();
              default:
                 write_buffer(&m);
                break;
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
    printf("waiting for new message!\n"); 
    int status = sef_receive(ANY, m_ptr);   /* blocks until message arrives */
    if (OK != status)
        panic("failed to receive message!: %d", status);
    who_e = m_ptr->m_source;        /* message arrived! set sender */
    callnr = m_ptr->m_type;       /* set function call number */
}

/*===========================================================================*
 *				do_ctl                                       *
 *===========================================================================*/
PRIVATE void do_ctl(
  message *m_ptr			/* message buffer */
)
        int r;
        printf("Setting internals1\n");
        ebp_relevant_buffer = m_ptr->EBP_RELBUF;
        bitmap              = m_ptr->PROS_BITMAP;

        if ((r = attach_memory((key_t) m_ptr->PROS_BUFFER1, (key_t) m_ptr->PROS_BUFFER2)) != OK)
                printf("PROS: failed to attach shared memory from consumer. Aborting profiling.\n");
        /*printf("Set internals, bitmap = %d, first = %d, second = %d, relbuf = %d\n",ebp_bm,ebp_first,ebp_second,ebp_relevant_buffer);
        printf("Set internals, relbuf = %d\n", ebp_relevant_buffer);*/
        
        return;
}


/*===========================================================================*
 *				attach_memory                                *
 *===========================================================================*/
PRIVATE int attach_memory(
 key_t key1,                              /* first shmem key  */
 key_t key2                               /* second shmem key */
)
    int shmid1, shmid2;
    /* get shared memory ids */
    if ((shmid1 = shmget(key1, IPC_CREAT, 0666)) < 0)
        return ENOMEM;
    if ((shmid2 = shmget(key2, IPC_CREAT, 0666)) < 0)
        return ENOMEM;

    /* attach shared memory */
    if ((first = shmat(shmid, NULL, 0)) == (char *) -1) {
        return ENOMEM;
    }
    if ((second = shmat(shmid, NULL, 0)) == (char *) -1) {
        return ENOMEM;
    }
    return OK;
}


/*===========================================================================*
 *				setreply				     *
 *===========================================================================*/
PUBLIC void setreply(proc_nr, result)
int proc_nr;			/* process to reply to */
int result;			/* result of call (usually OK or error #) */
{
/* Fill in a reply message to be sent later to a user process.  System calls
 * may occasionally fill in other fields, this is only for the main return
 * value, and for setting the "must send reply" flag.
 */
  register struct mproc *rmp = &mproc[proc_nr];

  if(proc_nr < 0 || proc_nr >= NR_PROCS)
      panic("setreply arg out of range: %d", proc_nr);

  rmp->mp_reply.reply_res = result;
  rmp->mp_flags |= REPLY;	/* reply pending */
}

/*===========================================================================*
 *				sendreply				     *
 *===========================================================================*/
PRIVATE void sendreply()
{
  int proc_nr;
  int s;
  struct mproc *rmp;

  /* Send out all pending reply messages, including the answer to
   * the call just made above.
   */
  for (proc_nr=0, rmp=mproc; proc_nr < NR_PROCS; proc_nr++, rmp++) {
      /* In the meantime, the process may have been killed by a
       * signal (e.g. if a lethal pending signal was unblocked)
       * without the PM realizing it. If the slot is no longer in
       * use or the process is exiting, don't try to reply.
       */
      if ((rmp->mp_flags & (REPLY | IN_USE | EXITING)) ==
          (REPLY | IN_USE)) {
          s=sendnb(rmp->mp_endpoint, &rmp->mp_reply);
          if (s != OK) {
              printf("PM can't reply to %d (%s): %d\n",
                  rmp->mp_endpoint, rmp->mp_name, s);
          }
          rmp->mp_flags &= ~REPLY;
      }
  }
}

