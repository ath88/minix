/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minix/ebp.h>
#include <minix/syslib.h>
#include <minix/callnr.h>

#if EBPROFILE

/* Until mutexes are implemented */
#define mutex_lock() (void)0
#define mutex_unlock() (void)0

int relevant_buffer;
ebp_buffers *buffers;

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  message m;
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();
  relevant_buffer = malloc(sizeof(int));

  /* Set profiling flag */
  bitmap &= 0x1;
 
  /* do syscall */ 
  m.EBP_BUFFER1	= buffers->first;
  m.EBP_BUFFER2	= buffers->second;
  m.EBP_RELBUF  = relevant_buffer;
  m.EBP_BITMAP	= bitmap;

  _syscall(PM_PROC_NR, EBPROF, &m);
  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  message m;
  m.EBP_BUFFER1	= NULL;
  m.EBP_BUFFER2	= NULL;
  m.EBP_BITMAP	= 0x0;
  m.EBP_RELBUF	= 0x0;
  free(buffers->first);
  free(buffers->second);
  free(relevant_buffer);
  _syscall(PM_PROC_NR, EBPROF, &m);
  return;
}

/* Write current profiling information to buffer. */
int
ebp_get (ebp_sample_buffer *buffer)
{ 
        unsigned int tmp, reached;
        ebp_sample_buffer *buf_ptr;

        mutex_lock();
        /* Change buffer */
        if (relevant_buffer)
        {
                relevant_buffer = 0;
                buf_ptr = &buffers->first; 
        }
        else
        {
                relevant_buffer = 1;
                buf_ptr = &buffers->second; 
        }
        reached = buf_ptr->reached;
        tmp = reached;

        (reached <= BUFFER_SIZE) ?: (reached = BUFFER_SIZE);
	memcpy(buffer, (void *)buf_ptr, sizeof(kcall_sample[reached]));

        buf_ptr->reached = 0;
        mutex_unlock();
        return tmp;
}

/* Allocates memory for double buffering */
ebp_sample_buffer *
alloc_buffers (void)
{
  kcall_sample *buffer;
  buffer = malloc (sizeof (ebp_sample_buffer));
  if (buffer == NULL)
    {
      printf("Could not allocate buffers. Disabling event-based profiling.\n");
    }
  else
    {
      memset (buffer, '\0', sizeof (ebp_sample_buffer));
    }
  return buffer;
}

#endif /* EBPROFILE */
