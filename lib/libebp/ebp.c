/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include <minix/ebp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minix/syslib.h>

#if EBPROFILE

EXTERN void *inactive_buffer;
EXTERN unsigned int switch_buffer;

ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
kcall_sample *alloc_buffers (void);

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  ebp_buffers *buffers;
  message m;
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();

  /* Set profiling flag */
  bitmap &= 0x1;
 
  /* do syscall */ 
  //sys_ebprof(buffers->first, buffers->second, bitmap);
  m.BUFFER1	= buffers->first;
  m.BUFFER2	= buffers->second;
  m.BITMAP	= bitmap;

  _syscall(PM_PROC_NR, EBPROF, &m);
  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  sys_ebprof(NULL, NULL, 0x0);
  return;
}


/* Write current profiling information to buffer. */
int
ebp_get (void *buffer)
{ 
  int switch_ret;
  if (switch_buffer)
  {
	buffer = inactive_buffer;
	switch_ret = switch_buffer;
	switch_buffer = 0;
	return switch_ret;
  }
  return switch_buffer;
}

/* Allocates memory for double buffering */
kcall_sample *
alloc_buffers (void)
{
  kcall_sample *buffer;
  buffer = malloc (sizeof (kcall_sample[BUFFER_SIZE]));
  if (buffer == NULL)
    {
      printf("Could not allocate buffers. Disabling event-based profiling.\n");
    }
  else
    {
      memset (buffer, '\0', sizeof (kcall_sample[BUFFER_SIZE]));
    }
  return buffer;
}

int buffer_switched (void)
{
	return switch_buffer;
}

#endif /* EBPROFILE */
