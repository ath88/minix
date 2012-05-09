/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minix/ebprofile.h>
#include <minix/syslib.h>

#if EBPROFILE

/*
ebp_buffers *ebp_start (int bitmap);
void ebp_stop (void);
int ebp_get (void *buffer);
kcall_sample *alloc_buffers (void);
int buffers_switched (void);
*/

int *switch_buffer;
ebp_buffers *buffers;

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  buffers->first  = alloc_buffers();
  buffers->second = alloc_buffers();
  switch_buffer = malloc(int);

  /* Set profiling flag */
  bitmap &= 0x1;
 
  /* do syscall */ 
  sys_ebprof(buffers->first, buffers->second, bitmap, switch_buffer); //MAYBE LOCKS?

  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  sys_ebprof(NULL, NULL, 0x0, NULL);
  return;
}


/* Write current profiling information to buffer. */
int
ebp_get (void *buffer)
{ 
  // SHOULD WE USE SLEEPQUEUES?

  int switch_ret;
  if (switch_buffer)
  {
        // NEED TO USE MEMCOPY TO BUFFER FROM RELEVANT BUFFER
//wrong?	buffer = inactive_buffer;
// 	instead check on % of switch buffer, remember to use locks
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
