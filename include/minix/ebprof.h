#ifndef EBPROF_H
#define EBPROF_H

/* This header file defines ...
*/

#include "com.h"
#if EBPROFILE

struct
{
  int time;
  int kcall;
  char *callee[256];
  char *params[256];
} kcall_sample;

int initialize (void);
int collect (char* kcall, int size);
int alloc_buffers (void);

#endif /* EBPROF_H */
