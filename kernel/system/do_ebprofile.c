/* The kernel call implemented in this file:
 *   m_type:	SYS_EBPROFILE
 *
 * The parameters for this kernel call are:
 *    m1_i1:	BUFFER1 	(first buffer for collected data)	
 *    m1_i2:	BUFFER2 	(second buffer for collected data)	
 */

#include "kernel/system.h"
#include <unistd.h>

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

  return(OK);

/*
  * See if the monitor is to run the specified instructions. *
  if (how == RBT_MONITOR) {
      int p;
      static char paramsbuffer[512];
      int len;
      len = MIN(m_ptr->ABRT_MON_LEN, sizeof(paramsbuffer)-1);

      if((p=data_copy(m_ptr->ABRT_MON_ENDPT, (vir_bytes) m_ptr->ABRT_MON_ADDR,
		KERNEL, (vir_bytes) paramsbuffer, len)) != OK) {
		return p;
      }
      paramsbuffer[len] = '\0';

      * Parameters seem ok, copy them and prepare shutting down. *
      if((p = arch_set_params(paramsbuffer, len+1)) != OK)
	return p;
  }

  * Now prepare to shutdown MINIX. *
  prepare_shutdown(how);
*/
}

#endif /* EBPROFILE */
