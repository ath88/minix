#ifndef EBPROF_H
#define EBPROF_H

/* This header file defines types and structures
*  for event-based profiling.
*/

#include "com.h"
#include "priv.h"
#include "kernel.h"

#if EBPROFILE

#define TRACE_PROBE(provider, 

struct
{
  int time;
  int kcall;
  int pid;
  int *params[12];
  /* from kernel/proc.c */
  unsigned cpu;
  char p_priority;
  struct priv *p_priv;  
} kcall_sample;

int initialize (void);
int collect (message *m_user, struct proc *caller);
int alloc_buffers (void);

#endif /* EBPROF_H */
