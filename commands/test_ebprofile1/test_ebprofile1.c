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
  int r;
  message m;
  printf("starting test-probes\n");
  m.m_type = 1337;
  while(1) 
  {
    printf("trying\n");
    r = sendrec(PM_PROC_NR, &m);
    sleep(1);
  }
}  
