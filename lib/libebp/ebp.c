/* Library implementing functions for 
*  event-based profiling in MINIX 3. 
*/

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <limits.h>
#include <lib.h>
#include <minix/config.h>
#include <minix/com.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/ipc.h>
#include <minix/rs.h>
#include <minix/syslib.h>
#include <minix/bitmap.h>
#include <paths.h>
#include <minix/sef.h>
#include <minix/dmap.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <configfile.h>

#include <machine/archtypes.h>
#include <timers.h>
#include <err.h>

#include "config.h"
#include "proto.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <lib.h>
#include <minix/ebprofile.h>
#include <minix/syslib.h>
#include <minix/callnr.h>
#include <minix/dmap.h>
#include <configfile.h>
#include <minix/rs.h>
#include <assert.h>

#include <sys/ipc.h>


#if EBPROFILE

#define OK 0

#define RUN_CMD         "run"
#define RUN_SCRIPT      "/etc/rs.single"        /* Default script for 'run' */
#define SELF_BINARY     "self"
#define SELF_REQ_PATH   "/dev/null"
#define PATH_CONFIG     _PATH_SYSTEM_CONF       /* Default config file */
#define DEFAULT_LU_STATE   SEF_LU_STATE_WORK_FREE /* Default lu state */
#define DEFAULT_LU_MAXTIME 0                    /* Default lu max time */


/* Until mutexes are implemented */
#define mutex_lock() (void)0
#define mutex_unlock() (void)0

PRIVATE char command[4096];

int ebprofiling : 1;
PRIVATE int req_type;
PRIVATE int do_run= 0;          /* 'run' command instead of 'up' */
PRIVATE char *req_label = NULL;
PRIVATE char *req_path = NULL;
PRIVATE char *req_path_self = SELF_REQ_PATH;
PRIVATE char *req_args = "";
PRIVATE int req_major = 0;
PRIVATE int devman_id = 0;
PRIVATE int req_dev_style = STYLE_NDEV;
PRIVATE long req_period = 0;
PRIVATE char *req_script = NULL;
PRIVATE char *req_config = PATH_CONFIG;
PRIVATE int custom_config_file = 0;
PRIVATE int req_lu_state = DEFAULT_LU_STATE;
PRIVATE int req_lu_maxtime = DEFAULT_LU_MAXTIME;

PRIVATE void failure(int request);

ebp_buffers *buffers;

int
start_ebp_server()
{
  printf("startserver 0\n");
  message m;
  int request = RS_UP;
  int result = EXIT_SUCCESS;
  char *progname = "pros";
  strcpy(command, "/usr/sbin/pros ");
  /* Arguments for RS to start a new service */
  struct rs_config config;
  u32_t rss_flags = 0;

  printf("startserver 1\n");

  assert(progname); /* an absolute path was required */

  printf("startserver 2\n");

  if (req_config) {
    assert(progname);
    memset(&config, 0, sizeof(config));
    if(!parse_config(progname, custom_config_file, req_config, &config))
    errx(1, "couldn't parse config");
  }

  printf("startserver 3\n");


  /* Set specifics */
  config.rs_start.rss_cmd= command;
  config.rs_start.rss_cmdlen= strlen(command);
  config.rs_start.rss_major= req_major;
  config.rs_start.rss_dev_style= req_dev_style;
  config.rs_start.rss_period= req_period;
  config.rs_start.rss_script= req_script;
  config.rs_start.devman_id= devman_id;
  config.rs_start.rss_flags |= rss_flags;
  if(req_label) {
   config.rs_start.rss_label.l_addr = req_label;
   config.rs_start.rss_label.l_len = strlen(req_label);
  } else {
    config.rs_start.rss_label.l_addr = progname;
    config.rs_start.rss_label.l_len = strlen(progname);
  }
  if (req_script)
    config.rs_start.rss_scriptlen= strlen(req_script);
  else
    config.rs_start.rss_scriptlen= 0; 
  assert(config.rs_start.rss_priority < NR_SCHED_QUEUES);
  assert(config.rs_start.rss_quantum > 0);

  printf("startserver 4\n");

  m.RS_CMD_ADDR = (char *) &config.rs_start;

  /* Build request message and send the request. */
  if(result == OK) {
    if (_syscall(RS_PROC_NR, request, &m) == -1)
      failure(request);
    result = m.m_type;
  }

  printf("startserver 5\n");

  return result;
}

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  unsigned int shmkey1, shmkey2, shmkey3;
  endpoint_t endpoint;

  /* Generate keys for shared memory */
  shmkey1 = 0x1234;
  shmkey2 = 0x5678;
  shmkey3 = 0x1945;

  if(start_ebp_server() == OK)
  {
    printf("ebpserver started\n");
  } else printf("ebpserver not startet\n");

  printf("ebpserver pid addr = %x\n",&endpoint);
  minix_rs_lookup("pros",&endpoint);

  printf("ebpserver pid addr = %x\n",&endpoint);
  printf("ebpserver pid = %d\n",endpoint);

  (void)fprintf(stdout,"LIB start1\n");
  message m;

  (void)fprintf(stdout,"LIB start101\n");
  buffers = malloc(sizeof(ebp_buffers));

  (void)fprintf(stdout,"LIB start11\n");
  buffers->first = alloc_buffers(shmkey1);

  (void)fprintf(stdout,"LIB start12\n");
  buffers->second = alloc_buffers(shmkey2);

  int shmid;
  (void)fprintf(stdout,"LIB start13\n");
  if ((shmid = shmget(shmkey3, sizeof (int), IPC_CREAT | 0666)) < 0) {
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
           return ENOMEM;
  }
  if ((buffers->relbuf = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
    exit(1);
  }
  (void)fprintf(stdout,"LIB start2\n");

  /* Set profiling flag */
  bitmap &= 0xFFF;
 
  (void)fprintf(stdout,"LIB start3\n");
  /* do syscall */ 
  m.PROS_BUFFER1	= shmkey1;
  m.PROS_BUFFER2	= shmkey2;
  m.PROS_RELBUF         = shmkey3;
  m.PROS_BITMAP	        = bitmap;
  (void)fprintf(stdout,"LIB start4 newer\n");
  sleep(1);
  _syscall(endpoint, PROS_CTL, &m);
  (void)fprintf(stdout,"LIB start5\n");
  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  message m;
  m.PROS_BUFFER1	= NULL;
  m.PROS_BUFFER2	= NULL;
  m.PROS_BITMAP		= 0x0;
  m.PROS_RELBUF		= 0x0;
  free(buffers->first);
  free(buffers->second);
  free(buffers->relbuf);
//  _syscall(PM_PROC_NR, EBPROF, &m);
  return;
}

/* Write current profiling information to buffer. */
int
ebp_get (ebp_buffers *buffer)
{ 
        unsigned int tmp, reached;
        ebp_sample_buffer *buf_ptr;

        mutex_lock();
        /* Change buffer */
        if (buffers->relbuf)
        {
                buffers->relbuf = 0;
                buf_ptr = &buffers->first; 
        }
        else
        {
                buffers->relbuf = 1;
                buf_ptr = &buffers->second; 
        }
        reached = buf_ptr->reached;
        tmp = reached;

        (reached <= BUFFER_SIZE) ?: (reached = BUFFER_SIZE);
	memcpy(buffer, (void *)buf_ptr, sizeof(kcall_sample[reached]));

        buf_ptr->reached = 0;
        mutex_unlock();
        return tmp;
}

/* Allocates memory for double buffering */
ebp_sample_buffer *
alloc_buffers (key_t key)
{
  fprintf(stdout,"allocB start\n");
  int shmid;
  char *buffer;

  if ((shmid = shmget(key, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) {
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
           return ENOMEM;
  }
  if ((buffer = shmat(shmid, NULL, 0)) == (char *)-1) {
           printf("Could not attach shared memory. Disabling event-based profiling.\n");
           return ENOMEM;
  }
  return (ebp_sample_buffer *)buffer;
}

/* A request to the RS server failed. Report and exit. 
 */
PRIVATE void failure(int request)
{
  fprintf(stderr, "Request 0x%x to RS failed: %s (error %d)\n", request, strerror(errno), errno);
  exit(errno);
}

/* Toggles event-based profiling.
 */
void
handle_ebpctl(void)
{
        (ebprofiling) ? (ebprofiling = 0) : (ebprofiling = 1);
        return;  
}

#endif /* EBPROFILE */
