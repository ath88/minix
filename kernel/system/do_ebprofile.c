/* The kernel call implemented in this file:
 *   m_type:	SYS_EBPROFILE
 *
 * The parameters for this kernel call are:
 *    m1_i1:	EB_BUFFER1 	(first buffer for collected data)	
 *    m1_i2:	EB_BUFFER2 	(second buffer for collected data)	
 *    m1_i3: 	EB_BITMAP	(bitmap specifying what to profile)
 */

#include "kernel/system.h"
#include <unistd.h>
#include "ebprofile.h"

#if EBPROFILE


/*===========================================================================*
 *				do_ebprofile				     *
 *===========================================================================*/
int do_ebprofile(struct proc * caller, message * m_ptr)
{
/* Handle sys_ebprofile.
*/
  void *buffer1 = m_ptr->EB_BUFFER1;
  void *buffer2 = m_ptr->EB_BUFFER2;
  int bitmap = m_ptr->EB_BITMAP;
 
  /* check if we are trying to disable profiling */ 
  if (m_ptr->EB_BUFFER1 == NULL &&
      m_ptr->EB_BUFFER1 == NULL && m_ptr->EB_BITMAP == 0)
  {
//    ebp_bm = 0x0;
    return(OK);
  }

  /* if not, enable and set buffer & flags appropriately */
//  first  = buffer1;
//  second = buffer2; 
//  ebp_bm = bitmap;

  return(OK);
}

#endif /* EBPROFILE */
