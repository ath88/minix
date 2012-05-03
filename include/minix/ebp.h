#ifndef EBPROF_H
#define EBPROF_H

/* This header file defines types and structures
*  for event-based profiling.
*/

#include "com.h"
#include "priv.h"
#include "kernel.h"

#if EBPROFILE

#define EBP_PM     1
#define EBP_SIGH   2
#define EBP_MEM    3
#define EBP_COPY   4
#define EBP_SHAD   5
#define EBP_DEVIO  6
#define EBP_SYSCTL 7
#define EBP_CLOCK  8
#define EBP_PROF   9

EXTERN int ebp_bm; /* bitmap for profiling flags */

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

int start (void);
void stop (void);
int get (void *buffer);
int initialize (void);
int collect (message *m_user, struct proc *caller);
int *alloc_buffers (void);

#endif /* EBPROF_H */
