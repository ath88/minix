#include "syslib.h"

/*===========================================================================*
 *                                sys_ebprof				     *
 *===========================================================================*/
PUBLIC int sys_ebprof(buffer1, buffer2, ebp_pm)
void *buffer1;
void *buffer2;
int ebp_pm;
{
        message m;

	m.EB_BUFFER1 	= buffer1;
	m.EB_BUFFER2 	= buffer2;
	m.EB_BITMAP	= ebp_pm;

        return(_kernel_call(SYS_EBPROF, &m));
}
