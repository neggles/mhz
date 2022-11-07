#include "mhz.h"

/* #define _DEBUG */

#if defined(HAVE_SYSMP)
#    include <sys/sysmp.h>
#    include <sys/sysinfo.h>
#endif

#if defined(HAVE_MPCTL)
#    include <sys/mpctl.h>
#endif

#if defined(HAVE_BINDPROCESSOR)
#    include <sys/processor.h>
#endif

#if defined(HAVE_PROCESSOR_BIND)
#    include <sys/types.h>
#    include <sys/processor.h>
#    include <sys/procset.h>
#endif

#if defined(HAVE_SCHED_SETAFFINITY)
#    include <sched.h>
#endif

extern int custom(char *str, int cpu);
extern int reverse_bits(int cpu);
extern int sched_ncpus();
extern int sched_pin(int cpu);

/*
 * The interface used by benchmp.
 *
 * childno is the "logical" child id number.
 *	In range [0, ..., parallel-1].
 * benchproc is the "logical" id within the benchmark process.  The
 *	benchmp-created process is logical ID zero, child processes
 *	created by the benchmark range from [1, ..., nbenchprocs].
 * nbenchprocs is the number of child processes that each benchmark
 * 	process will create.  Most benchmarks will leave this zero,
 *	but some such as the pipe() benchmarks will not.
 */
int handle_scheduler(int childno, int benchproc, int nbenchprocs) {
    int   cpu   = 0;
    char *sched = getenv("LMBENCH_SCHED");

    if (!sched || strcasecmp(sched, "DEFAULT") == 0) {
        /* do nothing.  Allow scheduler to control placement */
        return 0;
    } else if (strcasecmp(sched, "SINGLE") == 0) {
        /* assign all processes to CPU 0 */
        cpu = 0;
    } else if (strcasecmp(sched, "BALANCED") == 0) {
        /* assign each benchmark process to its own processor,
         * but child processes will share the CPU with the
         * parent.
         */
        cpu = childno;
    } else if (strcasecmp(sched, "BALANCED_SPREAD") == 0) {
        /*
         * assign each benchmark process to its own processor,
         * logically as far away from neighboring IDs as
         * possible.  This can help identify bus contention
         * issues in SMPs with hierarchical busses or NUMA
         * memory.
         */
        cpu = reverse_bits(childno);
    } else if (strcasecmp(sched, "UNIQUE") == 0) {
        /*
         * assign each benchmark process and each child process
         * to its own processor.
         */
        cpu = childno * (nbenchprocs + 1) + benchproc;
    } else if (strcasecmp(sched, "UNIQUE_SPREAD") == 0) {
        /*
         * assign each benchmark process and each child process
         * to its own processor, logically as far away from
         * neighboring IDs as possible.  This can help identify
         * bus contention issues in SMPs with hierarchical busses
         * or NUMA memory.
         */
        cpu = reverse_bits(childno * (nbenchprocs + 1) + benchproc);
    } else if (strncasecmp(sched, "CUSTOM ", strlen("CUSTOM ")) == 0) {
        cpu = custom(sched + strlen("CUSTOM"), childno);
    } else if (strncasecmp(sched, "CUSTOM_UNIQUE ", strlen("CUSTOM_UNIQUE ")) == 0) {
        cpu = custom(sched + strlen("CUSTOM_UNIQUE"), childno * (nbenchprocs + 1) + benchproc);
    } else {
        /* default action: do nothing */
        return 0;
    }

    return sched_pin(cpu % sched_ncpus());
}

/*
 * Use to get sequentially created processes "far" away from
 * each other in an SMP.
 *
 * XXX: probably doesn't work for NCPUS not a power of two.
 */
int reverse_bits(int cpu) {
    int i;
    int nbits;
    int max         = sched_ncpus() - 1;
    int cpu_reverse = 0;

    for (i = max >> 1, nbits = 1; i > 0; i >>= 1, nbits++)
        ;
    /* now reverse the bits */
    for (i = 0; i < nbits; i++) {
        if (cpu & (1 << i)) cpu_reverse |= (1 << (nbits - i - 1));
    }
    return cpu_reverse;
}

/*
 * Custom is a user-defined sequence of CPU ids
 */
int custom(char *str, int cpu) {
    static int  nvalues = -1;
    static int *values  = NULL;

    if (values == NULL) {
        nvalues = 0;
        values  = (int *)malloc(sizeof(int));

        while (*str) {
            char *q;
            while (*str && !isdigit(*str))
                str++;
            q = str;
            while (*str && isdigit(*str))
                str++;
            if (str == q) break;
            *str++ = 0;
            sscanf(q, "%d", &values[nvalues++]);
            values = (int *)realloc((void *)values, (nvalues + 1) * sizeof(int));
        }
    }
    if (nvalues == 0) return 0;
    return values[cpu % nvalues];
}

/*
 * Return the number of processors in this host
 */
int sched_ncpus() {
    /* AIX, Solaris, and Linux interface */
    return sysconf(_SC_NPROCESSORS_ONLN);
}

/*
 * Pin the current process to the given CPU
 *
 * return 0 when successful
 * returns -1 on error
 */
int sched_pin(int cpu) {
    int retval = -1;
    return retval;
}
