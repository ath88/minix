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

/*
ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
kcall_sample *alloc_buffers (void);
int buffers_switched (void);
*/

int *switch_buffer;
int relevant_buffer;
ebp_buffers *buffers;

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  message m;
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();
  switch_buffer = malloc(sizeof(int));

  /* Set profiling flag */
  bitmap &= 0x1;
 
  /* do syscall */ 
  m.EB_BUFFER1	= buffers->first;
  m.EB_BUFFER2	= buffers->second;
  m.EB_BITMAP	= bitmap;

  _syscall(PM_PROC_NR, EBPROF, &m);
  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  message m;
  m.EB_BUFFER1	= NULL;
  m.EB_BUFFER2	= NULL;
  m.EB_BITMAP	= 0x0;
  free(switch_buffer);
  free(buffers->first);
  free(buffers->second);
  switch_buffer = NULL;
  _syscall(PM_PROC_NR, EBPROF, &m);
  return;
}


/* Write current profiling information to buffer. */
int
ebp_get (void *buffer)
{ 
  // SHOULD WE USE SLEEPQUEUES?

  if (*switch_buffer)
  {
	int switch_ret = *switch_buffer;
	if (*switch_buffer % 2 == 1)
	{	
		relevant_buffer ? (relevant_buffer = 0) : (relevant_buffer = 1);
	}
	if (relevant_buffer)
	{
		memcpy(buffer, buffers->second, sizeof(kcall_sample[BUFFER_SIZE]));
	}
	else
	{
		memcpy(buffer, buffers->first, sizeof(kcall_sample[BUFFER_SIZE]));
	}
	*switch_buffer = 0;
	return switch_ret;
  }
  return *switch_buffer;
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
	return *switch_buffer;
}

#endif /* EBPROFILE */
