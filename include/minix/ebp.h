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

struct typedef
{
  void *first;
  void *second;
} ebp_buffers;

int ebp_start (void);
void ebp_stop (void);
int ebp_get (void *buffer);
int ebp_initialize (void);
int ebp_collect (message *m_user, struct proc *caller);
int *alloc_buffers (void);

#endif /* EBPROF_H */
