/* Kernel functions for event-based profiling
*/

#include "ebp.h"
#include "sys/sys/mutex.h"

#if EBPROFILE

extern int *first;
extern int *second;
extern int reached = 0; /* Where we are in the buffer */
extern struct kmutex buffer_mutex = NULL;
extern int *active_buffer;
extern int *inactive_buffer;

void set_ebprof(int bitmap);
int *active_buffer(void);
int ebprofiling(void);
int ebp_collect(message * m_user, struct proc *caller);

// TODO:
// BUFFER MANAGEMENT
// Make first/second in kernel, set first second buffers when system is called
// ebp.c fix binary
// BINARY macro?
// FIX EXTERN FIRST AND SECOND
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
active_buffer()
{
	int *tmp;
	
	/* swap if full active buffer or consumer is starving */
	if (reached == BUFFER_SIZE || time)
	{
		tmp = active_buffer;
		active_buffer = inactive_buffer;
		inactive_buffer = tmp;
		reached = 0;
		switch_buffer++;
	}
	else reached++;	
	return active_buffer;
}

/* Returns whether or not profiling is enabled */
int
ebprofiling()
{
	return ebp_bm & 1; // first bit
}

/* Write profiling information to buffer */
int
ebp_collect (message * m_user, struct proc *caller)
{
  int *current_buffer;
  struct kcall_sample *sample;

  if (buffer_mutex != NULL)
	mutex_destroy(buffer_mutex);	
  mutex_init(buffer_mutex, MUTEX_DEFAULT, IPL_NONE);
	
  /* Collect profiling data */ 
  sample->time		=
  sample->kcall 	=
  sample->p_nr 		= 
  sample->params 	=
  sample->cpu 		=
  sample->p_priority 	=
  sample->p_priv 	=

  current_buffer = active_buffer();
  current_buffer[reached] = sample;
  // Release mutex

  return 0;
}

#endif /* EBPROFILE */
