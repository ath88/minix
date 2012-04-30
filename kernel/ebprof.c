/* This file implements event-based profiling
 * in MINIX 3. 
*/

#include "ebprof.h"


int alloc_buffers (void);

int *pri_buf;
int *sec_buf;


/* Allocates memory for double buffering */
int
alloc_buffers ()
{
  pri_buf = malloc (BUFFER_SIZE);
  sec_buf = malloc (BUFFER_SIZE);

  if (pri_buf == 0 || sec_buf == 0)
    {
      printf
	("Could not allocate buffers. Disabling event-based profiling.\n")
	return 1;
    }
  else
    {
      memset (pri_buf, '\0', BUFFER_SIZE);
      memset (sec_buf, '\0', BUFFER_SIZE);
    }

}
