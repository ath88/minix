#include "syslib.h"

/*===========================================================================*
 *                                sys_ebprof				     *
 *===========================================================================*/
PUBLIC int sys_ebprof(buffer1, buffer2, ebp_pm, relevant_buffer)
void *buffer1;
void *buffer2;
int ebp_pm;
unsigned int *relevant_buffer;
{
        message m;

	m.EBP_BUFFER1 	= buffer1;
	m.EBP_BUFFER2 	= buffer2;
	m.EBP_RELBUF	= relevant_buffer;
	m.EBP_BITMAP	= ebp_pm;

        return(_kernel_call(SYS_EBPROF, &m));
}
