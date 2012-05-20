/* Profiling Service. 
 * This service implements functions for intercepting messages to the kernel
 * and gather event-based profiling data such as which kernel calls are invoked.
 */

#include "inc.h"	/* include master header file */
#include "glo.h"
#include <minix/endpoint.h>
#include <sys/shm.h>

/* Allocate space for the global variables. */
endpoint_t who_e, who_p;	/* caller's proc number */
int callnr;		/* system call number */
int bitmap;
ebp_sample_buffer *first;
ebp_sample_buffer *second;
unsigned int *relbuf;
int shmid1, shmid2, shmid3;

/* Declare some local functions. */
void get_work (message *m_ptr);
void reply (endpoint_t who_e, message *m_ptr);
void write_buffer (message *m_ptr);
int do_ctl (message *m_ptr);
int attach_memory (key_t key1, key_t key2, key_t key3);

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
                 reply(who_p, &m);
                 break;
              default:
                 write_buffer(&m);
                 reply(who_p, &m);
                break;
      }
  }
  return(OK);				/* shouldn't come here */
}

/*===========================================================================*
 *				write_buffer                                 *
 *===========================================================================*/
void write_buffer(
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
void get_work(
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
int do_ctl (
message *m_ptr			/* message buffer */
)
{
        int r;
        printf("Setting internals1\n");
        bitmap              = m_ptr->PROS_BITMAP;

        if ((r = attach_memory((key_t) m_ptr->PROS_BUFFER1, (key_t) m_ptr->PROS_BUFFER2, (key_t) m_ptr->PROS_RELBUF)) != OK)
                printf("PROS: failed to attach shared memory from consumer. Aborting profiling.\n");

        /*printf("Set internals, bitmap = %d, first = %d, second = %d, relbuf = %d\n",ebp_bm,ebp_first,ebp_second,ebp_relevant_buffer);
        printf("Set internals, relbuf = %d\n", ebp_relevant_buffer);*/
        return r;
}

/*===========================================================================*
 *				attach_memory                                *
 *===========================================================================*/
int attach_memory(
 key_t key1,                              /* shmem key for buffer 1 */
 key_t key2,                              /* shmem key for buffer 2 */
 key_t key3                               /* shmem key for relevant_buffer */
)
{
    int new_shmid1, new_shmid2, new_shmid3;
    /* get shared memory ids */
    if ((new_shmid1 = shmget(key1, IPC_CREAT, 0666)) < 0)
        return ENOMEM;
    if ((new_shmid2 = shmget(key2, IPC_CREAT, 0666)) < 0)
        return ENOMEM;
    if ((new_shmid3 = shmget(key3, IPC_CREAT, 0666)) < 0)
        return ENOMEM;
    
    /* are we trying to allocate new profiling buffers? */
    if (new_shmid1 == shmid1 && new_shmid2 == shmid2 && new_shmid3 == shmid3)
        return OK;

    /* attach shared memory */
    if ((first = shmat(shmid1, NULL, 0)) == (char *) -1) {
        return ENOMEM;
    }
    if ((second = shmat(shmid2, NULL, 0)) == (char *) -1) {
        return ENOMEM;
    }
    if ((relbuf = shmat(shmid3, NULL, 0)) == (char *) -1) {
        return ENOMEM;
    }
    return OK;
}

/*===========================================================================*
 *                              reply                                        *
 *===========================================================================*/
void reply(
endpoint_t who_e,                     /* destination */
message *m_ptr                        /* message buffer */
)
{
  int s = send(who_e, m_ptr);    /* send the message */
  if (OK != s)
    printf("PROS: unable to send reply to %d: %d\n", who_e, s);
}

