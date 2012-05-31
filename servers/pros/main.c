/* Profiling Service. 
 * This service implements functions for intercepting messages to the kernel
 * and gather event-based profiling data such as which kernel calls are invoked.
 */

#include "inc.h"	/* include master header file */
#include "glo.h"
#include <minix/endpoint.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>

/* Allocate space for the global variables. */
endpoint_t who_e;	/* caller's proc number */
int callnr;		        /* system call number */
int bitmap;
ebp_sample_buffer *first;
ebp_sample_buffer *second;
ebp_buffer_indicator *indicator;
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
  
  shmid1 = 0;
  shmid2 = 0;
  shmid3 = 0;

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
                 reply(who_e, &m);
                 do_ctl(&m);
                 break;
              case PROS_PROBE:
                 //Send reply before writing, to let process return fast as possible
                 reply(who_e, &m);
                 write_buffer(&m);
                 break;
              default:
                 // These are from servers, always sent async, no reply is expected
                 write_buffer(&m);
                 break;
      }
  }
  return(OK);				/* shouldn't get here */
}

/* SIMPLE BAD LOCK IMPLEMENTATION */
void
init_lock (int lock)
{
  lock = 0;
}

void
acquire (int lock) 
{
  while(lock == 1) 
    usleep(10);
  // this is where shit breaks  
  lock = 1;
}

void 
release (int lock)
{
  lock = 0;
}



/*===========================================================================*
 *				write_buffer                                 *
 *===========================================================================*/
void write_buffer(
  message *m_ptr			/* message buffer */
)
{
  ebp_sample_buffer *buffer;
  acquire(indicator->checkpoint);

  if (indicator->relbuf == 1) 
  {
    buffer = first;
  }
  else
  {
    buffer = second;
  }

  acquire(buffer->checkpoint);
  release(indicator->checkpoint);

  if (buffer->reached >= BUFFER_SIZE)
  {
    release(buffer->checkpoint);
    buffer->reached++; 
    return;
  }

  buffer->sample[buffer->reached].m_type = m_ptr->m_type;
  buffer->sample[buffer->reached].m_source = m_ptr->m_source;
  buffer->sample[buffer->reached].field = m_ptr->PROS_TYPE;
  buffer->sample[buffer->reached].payload = m_ptr->PROS_PAYLOAD;
  buffer->reached++; 

  release(buffer->checkpoint);

  return;
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
        bitmap              = m_ptr->PROS_BITMAP;

        if ((r = attach_memory((key_t) m_ptr->PROS_BUFFER1, (key_t) m_ptr->PROS_BUFFER2, (key_t) m_ptr->PROS_RELBUF)) != OK)
                printf("PROS: failed to attach shared memory from consumer. Can't profile.\n");

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
    /* get shared memory */
    if ((shmid1 = shmget(key1, sizeof(ebp_sample_buffer), 0666)) < 0)
        return ENOMEM;
    if ((shmid2 = shmget(key2, sizeof(ebp_sample_buffer), 0666)) < 0)
        return ENOMEM;
    if ((shmid3 = shmget(key3, sizeof(ebp_buffer_indicator), 0666)) < 0)
        return ENOMEM;
    
    /* attach shared memory */
    if ((first = shmat(shmid1, NULL, 0)) == (ebp_sample_buffer *) -1) {
        return ENOMEM;
    }
    if ((second = shmat(shmid2, NULL, 0)) == (ebp_sample_buffer *) -1) {
        return ENOMEM;
    }
    if ((indicator = shmat(shmid3, NULL, 0)) == (ebp_buffer_indicator *) -1) {
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
