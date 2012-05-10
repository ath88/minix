#ifndef _TOOL_EBPROF_H
#define _TOOL_EBPROF_H

/* This header file defines types and structures
*  for the ebprofile tool.
*/

#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/priv.h>
#include <minix/const.h>

#if EBPROFILE

#define HELP  0
#define START 1
#define STOP  2

#define STDOUT 0 
#define LFILE  1
#define NETW   2

#define BUFFER_SIZE 1024 // has to match with kernel/ebprofile.h

// COPY OF STRUCT FROM kernel/ebprofile.h !!!! 
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
  void *first;
  void *second;
} ebp_buffers;

/* userland functions */
ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
kcall_sample *alloc_buffers (void);
int buffer_switched (void);

#endif /* EBPROFILE */
#endif /* _TOOL_EBPROF_H */
