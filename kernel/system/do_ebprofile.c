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
#include <minix/ebp.h>

#if EBPROFILE

extern void *first; 
extern void *second; 
extern int *switch_buffer;

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
  unsigned int *ebp_switch;
 
  /* check if we are trying to disable profiling */ 
  if (m_ptr->EB_BUFFER1 == NULL &&
      m_ptr->EB_BUFFER1 == NULL && m_ptr->EB_BITMAP == 0)
  {
    set_ebprof(0x0);
    return(OK);
  }

  /* if not, enable and set buffer & flags appropriately */
  switch_buffer = m_ptr->EB_SWITCH;
  first  = buffer1;
  second = buffer2; 

  set_ebprof(bitmap);

  return(OK);
}

#endif /* EBPROFILE */
