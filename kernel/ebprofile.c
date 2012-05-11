/* Kernel functions for event-based profiling
*/

#include <minix/config.h>
#include "kernel.h"
#include "proc.h"
#include <stdio.h>

#if EBPROFILE

/* Until mutexes are implemented */
#define mutex_lock() (void)0
#define mutex_unlock() (void)0

void
set_internals(message *m_ptr)
{
(void)fprintf(stdout,"Set internals1");
        ebp_bm              = m_ptr->EBP_BITMAP;
        ebp_first           = (ebp_sample_buffer*) m_ptr->EBP_BUFFER1;
        ebp_second          = (ebp_sample_buffer*) m_ptr->EBP_BUFFER2;
        ebp_relevant_buffer = (unsigned int*) m_ptr->EBP_RELBUF;
	return;
}


/* Returns pointer to the next free slot in the active buffer */
kcall_sample
get_next_slot()
{
        kcall_sample free_sample;
        if (*ebp_relevant_buffer == 0)
        {
                free_sample = ebp_first->sample[ebp_first->reached];
                ebp_first->reached++;
        }
        else
        {
                free_sample = ebp_second->sample[ebp_second->reached];
                ebp_second->reached++;
        }
        return free_sample;
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
  kcall_sample sample;

  mutex_lock();
  sample = get_next_slot(); 

  /* Collect profiling data */ 	
  //sample.time		=
  sample.kcall 		= m_user->m_type; // This might be incorrect
  sample.p_nr 		= caller->p_nr;
  //sample.p_endpoint	= caller->p_endpoint;
  //sample.params 	=
  //sample.cpu 		= caller->p_cpu;
  //sample.p_priority 	= caller->p_priority;
  //sample.p_priv 	= caller->p_priv;

  mutex_unlock();
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
