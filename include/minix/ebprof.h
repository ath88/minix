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

#endif /* EBPROF_H */
