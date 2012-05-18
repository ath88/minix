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

  endpoint_t pros_proc_nr = 0;
  message m;

  m.m_type = 1337;

  printf("starting test-probes\n");
  while(1) 
  {
    printf("trying\n");

    if (pros_proc_nr == 0)
    {
      printf("pros server not found, looking up!\n");
      minix_rs_lookup("pros", &pros_proc_nr);
      if (pros_proc_nr != 0) 
        printf("pros server found, pid = %d\n",pros_proc_nr);
    }

    if (pros_proc_nr != 0) 
    {
      send(pros_proc_nr, &m);
      printf("message sent\n");
    }

    sleep(5);
  }
}  
