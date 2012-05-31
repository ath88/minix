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
    _syscall(RS_PROC_NR, request, &m);
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
    if(start_ebp_server() == OK)
    minix_rs_lookup("pros",&endpoint);
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
  m.RS_CMD_ADDR = "pros";
  m.RS_CMD_LEN = 4;
  _syscall(RS_PROC_NR, RS_DOWN, &m);

  /* pros server has been shut down now, deallocating shared memory */

  int shmid1, shmid2, shmid3;
  if ((shmid1 = shmget(SHMKEY1, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not get shmid during shutdown.\n");
  if ((shmid2 = shmget(SHMKEY2, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not get shmid during shutdown.\n");
  if ((shmid3 = shmget(SHMKEY3, sizeof (ebp_buffer_indicator), IPC_CREAT | 0666)) < 0) 
           printf("Could not get shmid during shutdown.\n");

  shmctl(shmid1, IPC_RMID, (struct shmid_ds*) &buffers->first);
  shmctl(shmid2, IPC_RMID, (struct shmid_ds*) &buffers->second);
  shmctl(shmid3, IPC_RMID, (struct shmid_ds*) &buffers->indicator);

  return;
}

void
init_lock (int lock)
{
  lock = 0;
}

void
acquire (int lock) 
{
  while(lock == 1) 
    usleep(10);
  // this is where shit breaks  
  lock = 1;
}

void 
release (int lock)
{
  lock = 0;
}


/* Write current profiling information to buffer. */
int
ebp_get (ebp_sample_buffer *buffer)
{ 
        unsigned int tmp, reached;
        ebp_sample_buffer *buf_ptr;

        acquire(buffers->indicator->checkpoint);
        if (buffers->indicator->relbuf == 1)
        {
                buffers->indicator->relbuf = 0;
                buf_ptr = buffers->first; 
        }
        else
        {
                buffers->indicator->relbuf = 1;
                buf_ptr = buffers->second; 
        }

        acquire(buf_ptr->checkpoint);
        release(buffers->indicator->checkpoint);

        reached = buf_ptr->reached;
        buf_ptr->reached = 0;
        tmp = reached;

        if (reached > BUFFER_SIZE)
          reached = BUFFER_SIZE;
        memcpy(buffer, (void *)buf_ptr->sample, sizeof(ebp_m_sample[reached]));

        release(buf_ptr->checkpoint);

        return tmp;
}

/* Allocates memory for double buffering */
void
alloc_buffers (void)
{

  fprintf(stdout,"allocB start\n");

  buffers = malloc(sizeof(ebp_buffers));

  int shmid1, shmid2, shmid3;
  if ((shmid1 = shmget(SHMKEY1, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
  if ((shmid2 = shmget(SHMKEY2, sizeof (ebp_sample_buffer), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");
  if ((shmid3 = shmget(SHMKEY3, sizeof (ebp_buffer_indicator), IPC_CREAT | 0666)) < 0) 
           printf("Could not allocate shared memory. Disabling event-based profiling.\n");

  if ((buffers->first = shmat(shmid1, NULL, 0)) == (ebp_sample_buffer*) -1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");
  if ((buffers->second = shmat(shmid2, NULL, 0)) == (ebp_sample_buffer*) -1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");
  if ((buffers->indicator = shmat(shmid3, NULL, 0)) == (ebp_buffer_indicator*) -1) 
           printf("Could not attach shared memory. Disabling event-based profiling.\n");

  init_lock(buffers->first->checkpoint); 
  init_lock(buffers->second->checkpoint); 
  init_lock(buffers->indicator->checkpoint); 

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

/* Toggles event-based profiling.
 */
void
handle_ebpctl(message *m)
{
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
