/* This file implements the entry point for event-based profiling.
 *
 * The entry points in this file are:
 *   do_ebprofile:   start/stop event-based profiling
 */

#include <minix/config.h>
#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

/*===========================================================================*
 *				do_ebprofile				     *
 *===========================================================================*/
PUBLIC int do_ebprofile(void)
{
#if EBPROFILE
	return sys_ebprof(m_in.EBP_BUFFER1, m_in.EBP_BUFFER2, m_in.EBP_BITMAP, m_in.EBP_RELBUF);
#else
	return ENOSYS;
#endif 
}
