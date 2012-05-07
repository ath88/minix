#include "syslib.h"

/*===========================================================================*
 *                                sys_ebprof				     *
 *===========================================================================*/
int sys_ebprof(buffer1, buffer2, ebp_pm)
void *buffer1;
void *buffer2;
int ebp_pm;
{
        message m;

	m.BUFFER1 	= buffer1;
	m.BUFFER2 	= buffer2;
	m.BITMAP	= ebp_pm;

        return(_kernel_call(SYS_EBPROF, &m));
}
