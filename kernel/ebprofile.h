#ifndef _KE_EBPROF_H
#define _KE_EBPROF_H

/* This header file defines types and structures
*  for event-based profiling.
*/

#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/priv.h>
#include <minix/const.h>
#include "kernel.h"
#include "proc.h"

#if EBPROFILE

#define FLAG_AT(x) 0x1 << (x-1)
#define EBP_PM     FLAG_AT(1)
#define EBP_SIGH   FLAG_AT(2)
#define EBP_MEM    FLAG_AT(3)
#define EBP_COPY   FLAG_AT(4)
#define EBP_DEVIO   FLAG_AT(5)
#define EBP_CLOCK  FLAG_AT(6)
#define EBP_SYSCTL FLAG_AT(7)
#define EBP_PROF  FLAG_AT(8)
#define EBP_SHAD   FLAG_AT(9)
#define EBP_MISC   FLAG_AT(10)

#define BUFFER_SIZE	1024

/* Event-based profiling */
EXTERN int ebp_bm; /* bitmap */
EXTERN void *first;
EXTERN void *second;
EXTERN unsigned int *relevant_buffer;

typedef struct
{
  int time;
  int kcall;
  unsigned int p_nr;
  endpoint_t p_endpoint;
  int *params[12];
  /* from kernel/proc.c */
  unsigned cpu;
  char p_priority;
  struct priv *p_priv;  
} kcall_sample;

typedef struct
{
  kcall_sample sample[BUFFER_SIZE];
  unsigned int reached;
  int lock;
} ebp_sample_buffer;

/* kernel functions */
void set_ebprof(int bitmap);
void *get_active_buffer(void);
int ebprofiling(void);
int ebp_collect (message * m_user, struct proc *caller);
int matches_bm(int m_type);

#endif /* EBPROFILE */
#endif /* _KE_EBPROF_H */
