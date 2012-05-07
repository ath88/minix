#ifndef _EBPROF_H
#define _EBPROF_H

/* This header file defines types and structures
*  for event-based profiling.
*/

#include "com.h"
#include "priv.h"
#include "kernel.h"

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

EXTERN void *inactive_buffer;
EXTERN unsigned int switch_buffer;

struct
{
  int time;
  int kcall;
  proc_nr_t p_nr;
  endpoint_t p_endpoint;
  int *params[12];
  /* from kernel/proc.c */
  unsigned cpu;
  char p_priority;
  struct priv *p_priv;  
} kcall_sample;

typedef struct 
{
  void *first;
  void *second;
} ebp_buffers;

/* kernel functions */
void set_ebprof(int bitmap);
void *get_active_buffer(void);
void ebprofiling(void);
int ebp_collect (message *m_user, struct proc *caller);
int matches_bm(int m_type);

/* userland functions */
int ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
int *alloc_buffers (void);
int buffer_switched (void);

#endif /* EBPROFILE */
#endif /* _EBPROF_H */
