/* Kernel functions for event-based profiling
*/

#include "ebp.h"
#include "sys/sys/mutex.h"
#include "glo.h"

#if EBPROFILE

EXTERN void *first;
EXTERN void *second;
EXTERN int reached = 0; /* Where we are in the buffer */
EXTERN unsigned int switch_buffer;
EXTERN void *active_buffer;

void set_ebprof(int bitmap);
void *get_active_buffer(void);
int ebprofiling(void);
int ebp_collect(message * m_user, struct proc *caller);
int matches_bm(void);

// TODO:
// BUFFER MANAGEMENT
// Make first/second in kernel, set first second buffers when system is called
// READ DTRACE DOCS
// Shared locks + shared buffer = happiness
//     Need to get clock in kernel and userland
// LOOK AT ALL COMMENTS
// Disable interrupts

void
set_ebprof(int bitmap)
{
	ebp_bm = bitmap;
	return;
}

/* Returns pointer to active buffer */
int*
get_active_buffer()
{
	//MUTEX
	int *tmp;
	/* swap if full active buffer or consumer is starving */
	if (reached == BUFFER_SIZE) //|| time()))
	{
		tmp = active_buffer;
		active_buffer = inactive_buffer;
		inactive_buffer = tmp;
		reached = 0;
		switch_buffer++;
	}
	else reached++;	
	//MUTEX ULOCK
	return active_buffer;
}

/* Returns whether or not profiling is enabled */
int
ebprofiling()
{
	return ebp_bm & 0x1; // first bit
}

/* Write profiling information to buffer */
int
ebp_collect (message * m_user, struct proc *caller)
{
  int *current_buffer;
  struct kcall_sample *sample;

  /* Collect profiling data */ 
  int m_type = m_user->m_type; // ???

  current_buffer = get_active_buffer(); 
  sample = *current_buffer[reached];

  //sample->time		=
  sample->kcall 	= m_user->m_type; // This might be incorrect
  sample->p_nr 		= caller->p_nr;
  sample->p_endpoint	= caller->p_endpoint;
  //sample->params 	=
  sample->cpu 		= caller->p_cpu;
  sample->p_priority 	= caller->p_priority;
  //sample->p_priv 	= caller->p_priv;
  return 0;
}

int matches_bm()
{
	return 0;
}

#endif /* EBPROFILE */
