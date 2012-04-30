/* This file implements event-based profiling
 * in MINIX 3. 
 */

#include "ebprof.h"

#if EBPROFILE

int start (void);
int collect (void);
int alloc_buffers (void);

int buf_size = 0;
extern int *pri_buf;
extern int *sec_buf;

/* Starts event-based profiling. */
int
main(int argc, char* argv[])
{
	handle_args(argc, argv);
}

/* Initializes datastructures used for profiling. */
int
start()
{
	pri_buf = alloc_buffers();
	sec_buf = alloc_buffers();

	/* Start profiling in kernel */	
	return 0;
}

/* Write profiling information to buffer */
int
collect (message *m_user, struct proc *caller)
{
	return 0;
}

/* Handle arguments */
int
handle_args(int argc, char* argv[])
{

while (--argc) {
        ++argv;

        if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "help") == 0 ||
                strcmp(*argv, "--help") == 0) {
                return EHELP;
        } else
        if (strcmp(*argv, "-m") == 0) {
                if (--argc == 0) return ESYNTAX;
                if (sscanf(*++argv, "%u", &mem_size) != 1 ||
                        mem_size < MIN_MEMSIZE ) return EMEM;
        } else
        if (strcmp(*argv, "-f") == 0) {
                if (--argc == 0) return ESYNTAX;
                if (sscanf(*++argv, "%u", &freq) != 1)
                        return EFREQ;
        } else
        if (strcmp(*argv, "-o") == 0) {
                if (--argc == 0) return ESYNTAX;
                outfile = *++argv;
        } else
        if (strcmp(*argv, "--rtc") == 0) {
                intr_type = PROF_RTC;
        } else
        if (strcmp(*argv, "--nmi") == 0) {
                intr_type = PROF_NMI;
        } else
        if (strcmp(*argv, "start") == 0) {
                if (action) return EACTION;
                action = START;
        } else
        if (strcmp(*argv, "stop") == 0) {
                if (action) return EACTION;
                action = STOP;
        } else
        if (strcmp(*argv, "get") == 0) {
                if (action) return EACTION;
                action = GET;
}

}

#endif
