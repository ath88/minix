#include <minix/com.h>
#include <minix/ipc.h>
#include <minix/sysutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lib.h>
#include <minix/rs.h>

int
main(int argc, char *argv[])
{
  printf("starting test-probes\n");
  while(1) 
  {
    message m;
    m.m_type = PROS_SERVER_CTL;
    m.PROS_SERV_BMAP = 1;
    printf("trying on\n");
    sendrec(PM_PROC_NR, &m);
    sleep(4);
    printf("trying off\n");
    message m2;
    m2.m_type = PROS_SERVER_CTL;
    m2.PROS_SERV_BMAP = 0;
    sendrec(PM_PROC_NR, &m2);
    sleep(4);
  }
}  
