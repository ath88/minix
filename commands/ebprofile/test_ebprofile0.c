#include <minix/com.h>
#include <minix/ipc.h>
#include <minix/sysutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lib.h>
#include <minix/rs.h>
#include <minix/ebprofile.h>

int
main(int argc, char *argv[])
{
  printf("starting test-probes\n");
  while(1) 
  {
    printf("trying\n");

    sleep(2);
    probe("SQLQuery","Start");
    probe("SQLQuery","Stop");
  }
}  
