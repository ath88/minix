/* Kernel functions for event-based profiling
*/

#include "ebp.h"

#if EBPROFILE

extern int *first;
extern int *second;

// TODO:
// BUFFER MANAGEMENT
// Make first/second in kernel, set first second buffers when system is called
// ebp.c fix binary
// fix binary in header
// fix prototypes
// FIX EXTERN FIRST AND SECOND

int
set_ebprof(bitmap)
{
	// SET BITMAP
}

/* Returns pointer to active buffer */
int*
active_buffer()
{

}

/* Returns pointer to inactive buffer */
int*
inactive_buffer()
{

}

/* Returns whether or not profiling is enabled */
int
ebprofiling()
{
	return ebp_bm & 1; // first bit
}

/* Write profiling information to buffer */
int
ebp_collect (message * m_user, struct proc *caller)
{
  // This function has access to first & second!
  
  // Write to active buffer
  return 0;
}

#endif /* EBPROFILE */
