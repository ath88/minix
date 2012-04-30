#ifndef EBPROF_H
#define EBPROF_H

/* This header file defines ...
*/

#include "com.h"
#include "priv.h"

#if EBPROFILE

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
int collect (char* kcall, int size);
int alloc_buffers (void);

#endif /* EBPROF_H */
