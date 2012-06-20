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
#include <minix/mthread.h>
#include <sys/types.h>

#define HELP  0
#define START 1
#define STOP  2

#define STDOUT 0 
#define LFILE  1
#define NETW   2

#define BUFFER_SIZE 1024

typedef struct
{
  int m_type;
  unsigned int m_source;
  endpoint_t m_destination;
  int field; 
  int payload; 
  char typetext[32];
  char datatext[32];
} ebp_m_sample;

typedef struct
{
  ebp_m_sample sample[BUFFER_SIZE];
  unsigned int reached;
  int checkpoint;
} ebp_sample_buffer;

typedef struct
{
  unsigned int relbuf;
  int checkpoint;
} ebp_buffer_indicator;

typedef struct
{
  ebp_sample_buffer *first;
  ebp_sample_buffer *second;
  ebp_buffer_indicator *indicator;
} ebp_buffers;

/* userspace probe */
void probe(char* type, char* payload);

/* userspace consumer functions */
ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (ebp_sample_buffer *buffer);

/* library functions */
void alloc_buffers (void);

/* serverspace functions */
int handle_ebpctl(message *m);
void server_probe(message *m);

#endif /* _TOOL_EBPROF_H */
