/* Kernel functions for event-based profiling
*/

#include "ebp.h"
#include "sys/sys/mutex.h"

#if EBPROFILE

extern int *first;
extern int *second;
extern int reached; /* Where we are in the buffer */
extern struct kmutex buffer_mutex = NULL;

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
	if (buffer_mutex != NULL)
		mutex_destroy(buffer_mutex);	
	mutex_init(buffer_mutex, MUTEX_DEFAULT, IPL_NONE);

	ebp_bm = bitmap;
	return;
}

/* Returns pointer to active buffer */
int*
active_buffer()
{
	int *active_buffer;
	// Do timer stuff to determine which buffer is going to be written to
	// Lock it
	
	
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
  reached++;
  // Release mutex

  return 0;
}

#endif /* EBPROFILE */
