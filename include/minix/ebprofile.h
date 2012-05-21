#ifndef _TOOL_EBPROF_H
#define _TOOL_EBPROF_H

/* This header file defines types and structures
*  for the ebprofile tool.
*/

#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/priv.h>
#include <minix/const.h>
#include <sys/types.h>

#define HELP  0
#define START 1
#define STOP  2

#define STDOUT 0 
#define LFILE  1
#define NETW   2

#define BUFFER_SIZE 1024 // has to match with kernel/ebprofile.h

typedef struct
{
  int time;
  int m_type;
  unsigned int m_source;
  endpoint_t m_destination;
  int field; 
  int payload; 

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


typedef struct
{
  ebp_sample_buffer *first;
  ebp_sample_buffer *second;
  unsigned int *relbuf;
} ebp_buffers;

/* userland functions */
ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (ebp_sample_buffer *buffer);
ebp_sample_buffer *alloc_buffers (key_t key);
void probe(int type, int payload);

#endif /* _TOOL_EBPROF_H */
