/* Kernel functions for event-based profiling
*/

#include <minix/ebp.h>
#include "glo.h"

#if EBPROFILE

/* Until mutexes are implemented */
#define mutex_lock() (void)0
#define mutex_unlock() (void)0

EXTERN void *first;
EXTERN void *second;
EXTERN int reached; /* Where we are in the buffer */
unsigned int switch_buffer;
void *active_buffer;
void *inactive_buffer;

void set_ebprof(int bitmap);
void *get_active_buffer(void);
int ebprofiling(void);
int ebp_collect(message * m_user, struct proc *caller);
int matches_bm(int m_type);

// TODO:
// BUFFER MANAGEMENT
// Shared locks + shared buffer = happiness
//     Need to get clock in kernel and userland
// Disable interrupts

void
set_ebprof(int bitmap)

{
	reached = 0;
	ebp_bm = bitmap;
	return;
}

/* Returns pointer to active buffer */
void *
get_active_buffer()
{
	int *tmp;
	mutex_lock();

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
	mutex_unlock();
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
  void *current_buffer;
  kcall_sample sample;

  /* Collect profiling data */ 	
  int m_type = m_user->m_type;

  current_buffer = get_active_buffer(); 
  sample = ((kcall_sample*) current_buffer)[reached];

  //sample.time		=
  sample.kcall 		= m_user->m_type; // This might be incorrect
  sample.p_nr 		= caller->p_nr;
  //sample.p_endpoint	= caller->p_endpoint;
  //sample.params 	=
  //sample.cpu 		= caller->p_cpu;
  //sample.p_priority 	= caller->p_priority;
  //sample.p_priv 	= caller->p_priv;

  return 0;
}

/* A not so pretty filtering function */
int matches_bm(int m_type)
{
	if (0 <= m_type <= 6 || m_type == 34 || m_type == 46)
		return ebp_bm & EBP_PM;
	if (7 <= m_type <= 10)
		return ebp_bm & EBP_SIGH;
	if (11 <= m_type <=13 || m_type == 43)
		return ebp_bm & EBP_MEM;	
	if (14 <= m_type <=17 || 31 <= m_type <= 33)
		return ebp_bm & EBP_COPY;
	if (19 <= m_type <=23 ||m_type== 28 ||m_type == 35)
		return ebp_bm & EBP_DEVIO;
	if (24 <= m_type <= 25 || m_type == 39 || m_type == 45)
		return ebp_bm & EBP_CLOCK;
	if (26 <= m_type <= 27 || m_type == 44)
		return ebp_bm & EBP_SYSCTL;
	if (36 <= m_type <= 38)
		return ebp_bm & EBP_PROF;
	if (47 <= m_type <= 49)
		return ebp_bm & EBP_SHAD;
	if (50 <= m_type <= 55)
		return ebp_bm & EBP_MISC;
	return 0;
}

#endif /* EBPROFILE */
