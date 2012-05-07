/* The kernel call implemented in this file:
 *   m_type:	SYS_EBPROFILE
 *
 * The parameters for this kernel call are:
 *    m1_i1:	BUFFER1 	(first buffer for collected data)	
 *    m1_i2:	BUFFER2 	(second buffer for collected data)	
 *    m1_i3: 	BITMAP		(bitmap specifying what to profile)
 */

#include "kernel/system.h"
#include <unistd.h>
#include <ebp.h>

#if EBPROFILE

/*===========================================================================*
 *				do_ebprofile				     *
 *===========================================================================*/
int do_ebprofile(struct proc * caller, message * m_ptr)
{
/* Handle sys_ebprofile.
*/
  int buffer1 = m_ptr->BUFFER1;
  int buffer2 = m_ptr->BUFFER2;
  int bitmap = m_ptr->BITMAP;
 
  /* check if we are trying to disable profiling */ 
  if (m_ptr->BUFFER1 == NULL &&
      m_ptr->BUFFER1 == NULL && m_ptr->BITMAP == NULL)
  {
    set_ebprof(0x0);
    first = NULL;
    second = NULL;
    active_buffer = NULL;
    inactive_buffer = NULL;

    return(OK);
  }

  /* if not, enable and set buffer & flags appropriately */
  active_buffer = buffer1;
  inactive_buffer = buffer2;
  first  = buffer1;
  second = buffer2; 

  set_ebprof(bitmap);

  return(OK);
}

#endif /* EBPROFILE */
