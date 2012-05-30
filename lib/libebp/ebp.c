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
#include <sys/shm.h>
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

#include <minix/mthread.h>


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

#define SHMKEY1 0x3451
#define SHMKEY2 0x3452
#define SHMKEY3 0x3453
#define OK 0

#define RUN_CMD         "run"
#define RUN_SCRIPT      "/etc/rs.single"        /* Default script for 'run' */
#define SELF_BINARY     "self"
#define SELF_REQ_PATH   "/dev/null"
#define PATH_CONFIG     _PATH_SYSTEM_CONF       /* Default config file */
#define DEFAULT_LU_STATE   SEF_LU_STATE_WORK_FREE /* Default lu state */
#define DEFAULT_LU_MAXTIME 0                    /* Default lu max time */


PRIVATE char command[4096];

int ebprofiling = 0;
PRIVATE char *req_label = NULL;
PRIVATE int req_major = 0;
PRIVATE int devman_id = 0;
PRIVATE int req_dev_style = STYLE_NDEV;
PRIVATE long req_period = 0;
PRIVATE char *req_script = NULL;
PRIVATE char *req_config = PATH_CONFIG;
PRIVATE int custom_config_file = 0;

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

  assert(progname); /* an absolute path was required */

  if (req_config) {
    assert(progname);
    memset(&config, 0, sizeof(config));
    if(!parse_config(progname, custom_config_file, req_config, &config))
    errx(1, "couldn't parse config");
  }

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

  m.RS_CMD_ADDR = (char *) &config.rs_start;

  /* Build request message and send the request. */
  if(result == OK) {
    if (_syscall(RS_PROC_NR, request, &m) == -1)
      failure(request);
    result = m.m_type;
  }

  return result;
}

/* Initializes datastructures used for profiling. */
ebp_buffers *
ebp_start (int bitmap)
{
  endpoint_t endpoint = 0;
  minix_rs_lookup("pros",&endpoint);
  if (endpoint == 0) 
  {
    printf("PROS server not found, starting\n");
    if(start_ebp_server() == OK)
      printf("PROS server started\n");
    minix_rs_lookup("pros",&endpoint);
  }
  else
  {
    printf("PROS server found\n");
  }

  alloc_buffers();

  /* Set profiling flag */
  bitmap &= 0xFFF;

  /* tell pros server to start */ 
  message m;
  m.PROS_BUFFER1	= SHMKEY1;
  m.PROS_BUFFER2	= SHMKEY2;
  m.PROS_RELBUF         = SHMKEY3;
  m.PROS_BITMAP	        = bitmap;
  _syscall(endpoint, PROS_CTL, &m);

  /* tell pm to start profiling */
  m.PROS_SERV_BMAP         = 1; 
//  _syscall(PM_PROC_NR, PROS_SERVER_CTL, &m);

  return buffers;
}

/*  Stops profiling. */
void
ebp_stop (void)
{
  message m;
  m.PROS_BUFFER1	= 0;
  m.PROS_BUFFER2	= 0;
  m.PROS_BITMAP		= 0;
  m.PROS_RELBUF		= 0;
//  free(buffers->first);
//  free(buffers->second);
//  free(buffers->relbuf);
//  _syscall(PM_PROC_NR, EBPROF, &m);
  return;
}

/* Write current profiling information to buffer. */
int
ebp_get (ebp_sample_buffer *buffer)
{ 
        unsigned int tmp, reached;
        ebp_sample_buffer *buf_ptr;


        mthread_rwlock_wrlock(&buffers->indicator->lock);
        if (buffers->indicator->relbuf == 1)
        {
                buffers->indicator->relbuf = 0;
                buf_ptr = buffers->first; 
                mthread_rwlock_unlock(&buffers->second->lock);
        }
        else
        {
                buffers->indicator->relbuf = 1;
                buf_ptr = buffers->second; 
                mthread_rwlock_unlock(&buffers->first->lock);
        }

        mthread_rwlock_wrlock(&buf_ptr->lock);
        mthread_rwlock_unlock(&buffers->indicator->lock);


        memcpy(buffer, (void *)buf_ptr->sample, sizeof(ebp_m_sample[reached]));

        reached = buf_ptr->reached;
        buf_ptr->reached = 0;
        tmp = reached;

        if (reached > BUFFER_SIZE)
          reached = BUFFER_SIZE;

        return tmp;
}

/* Allocates memory for double buffering */
void
alloc_buffers (void)
{
  int shmid1, shmid2, shmid3;

  fprintf(stdout,"allocB start\n");

  buffers = malloc(sizeof(ebp_buffers));

  if ((shmid1 = shmget(SHMKEY1, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
  if ((shmid2 = shmget(SHMKEY2, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
  if ((shmid3 = shmget(SHMKEY3, sizeof (ebp_buffer_indicator), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");

  if ((buffers->first = shmat(shmid1, NULL, 0)) == (char *)-1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");
  if ((buffers->second = shmat(shmid2, NULL, 0)) == (char *)-1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");
  if ((buffers->indicator = shmat(shmid3, NULL, 0)) == (char *)-1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");

  mthread_rwlock_init(&buffers->first->lock); 
  mthread_rwlock_init(&buffers->second->lock); 
  mthread_rwlock_init(&buffers->indicator->lock); 

  return;
}

void
probe (int type, int payload)
{
  int pros_proc_nr = 0;
  minix_rs_lookup("pros", &pros_proc_nr);
  if (pros_proc_nr == 0) 
    return;                 // returning if we cant find pros server

  message m;
  m.m_type = PROS_PROBE;
  m.PROS_TYPE = type;
  m.PROS_PAYLOAD = payload;
  sendrec(pros_proc_nr, &m);
  // message is sent, we need to wait for response

  return;
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
handle_ebpctl(message *m)
{
        printf("PM:handling ebpctl: %d\n",m->PROS_SERV_BMAP); 
        ebprofiling = m->PROS_SERV_BMAP;
        return;  
}

void server_probe(message *m)
{
        if(ebprofiling == 1) 
        {
          endpoint_t proc_nr = 0;
          minix_rs_lookup("pros", &proc_nr);
          if (proc_nr == 0)
            return;
          sendnb(proc_nr, m);
        }
        return;
}

#endif /* EBPROFILE */
