/*
 * $Id$
 */
#ifndef _BENCH_H
#define _BENCH_H

#include <assert.h>
#include <ctype.h>

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>

#include <stdarg.h>

typedef unsigned int  uint;
typedef unsigned long uint64;

typedef long long     int64;


#include "libstats.h"
#include "libtiming.h"


#ifdef DEBUG
#    define debug(x) fprintf x
#else
#    define debug(x)
#endif


/*
 * socket send/recv buffer optimizations
 */
#define SOCKOPT_READ  0x0001
#define SOCKOPT_WRITE 0x0002
#define SOCKOPT_RDWR  0x0003
#define SOCKOPT_PID   0x0004
#define SOCKOPT_REUSE 0x0008
#define SOCKOPT_NONE  0

#ifndef SOCKBUF
#    define SOCKBUF (1024 * 1024)
#endif

#ifndef XFERSIZE
#    define XFERSIZE (64 * 1024) /* all bandwidth I/O should use this */
#endif

#define gettime usecs_spent
#define streq   !strcmp
#define ulong   unsigned long

#ifndef HAVE_DRAND48
#    ifdef HAVE_RAND
#        define srand48   srand
#        define drand48() ((double)rand() / (double)RAND_MAX)
#    elif defined(HAVE_RANDOM)
#        define srand48   srandom
#        define drand48() ((double)random() / (double)RAND_MAX)
#    endif /* HAVE_RAND */
#endif     /* HAVE_DRAND48 */

#define SMALLEST_LINE 32 /* smallest cache line size */
#define TIME_OPEN2CLOSE

#define GO_AWAY            \
    signal(SIGALRM, exit); \
    alarm(60 * 60);
#define REAL_SHORT 50000
#define SHORT      1000000
#define MEDIUM     2000000
#define LONGER     7500000 /* for networking data transfers */
#define ENOUGH     REAL_SHORT

#define TRIES      11

typedef struct {
    uint64 u;
    uint64 n;
} value_t;

typedef struct {
    int     N;
    value_t v[TRIES];
} result_t;
int       sizeof_result(int N);
void      insertinit(result_t *r);
void      insertsort(uint64, uint64, result_t *);
void      save_median();
void      save_minimum();
void      set_results(result_t *r);
result_t *get_results();


#define BENCHO(loop_body, overhead_body, enough)                            \
    {                                                                       \
        int      __i, __N;                                                  \
        double   __oh;                                                      \
        result_t __overhead, __r;                                           \
        insertinit(&__overhead);                                            \
        insertinit(&__r);                                                   \
        __N = (enough == 0 || get_enough(enough) <= 100000) ? TRIES : 1;    \
        if (enough < LONGER) {                                              \
            loop_body;                                                      \
        } /* warm the cache */                                              \
        for (__i = 0; __i < __N; ++__i) {                                   \
            BENCH1(overhead_body, enough);                                  \
            if (gettime() > 0) insertsort(gettime(), get_n(), &__overhead); \
            BENCH1(loop_body, enough);                                      \
            if (gettime() > 0) insertsort(gettime(), get_n(), &__r);        \
        }                                                                   \
        for (__i = 0; __i < __r.N; ++__i) {                                 \
            __oh = __overhead.v[__i].u / (double)__overhead.v[__i].n;       \
            if (__r.v[__i].u > (uint64)((double)__r.v[__i].n * __oh))       \
                __r.v[__i].u -= (uint64)((double)__r.v[__i].n * __oh);      \
            else                                                            \
                __r.v[__i].u = 0;                                           \
        }                                                                   \
        *(get_results()) = __r;                                             \
    }

#define BENCH(loop_body, enough)                                         \
    {                                                                    \
        long     __i, __N;                                               \
        result_t __r;                                                    \
        insertinit(&__r);                                                \
        __N = (enough == 0 || get_enough(enough) <= 100000) ? TRIES : 1; \
        if (enough < LONGER) {                                           \
            loop_body;                                                   \
        } /* warm the cache */                                           \
        for (__i = 0; __i < __N; ++__i) {                                \
            BENCH1(loop_body, enough);                                   \
            if (gettime() > 0) insertsort(gettime(), get_n(), &__r);     \
        }                                                                \
        *(get_results()) = __r;                                          \
    }

#define BENCH1(loop_body, enough)                         \
    {                                                     \
        double __usecs;                                   \
        BENCH_INNER(loop_body, enough);                   \
        __usecs = gettime();                              \
        __usecs -= t_overhead() + get_n() * l_overhead(); \
        settime(__usecs >= 0. ? (uint64)__usecs : 0);     \
    }

#define BENCH_INNER(loop_body, enough)                                     \
    {                                                                      \
        static iter_t __iterations = 1;                                    \
        int           __enough     = get_enough(enough);                   \
        iter_t        __n;                                                 \
        double        __result = 0.;                                       \
                                                                           \
        while (__result < 0.95 * __enough) {                               \
            start(0);                                                      \
            for (__n = __iterations; __n > 0; __n--) {                     \
                loop_body;                                                 \
            }                                                              \
            __result = stop(0, 0);                                         \
            if (__result < 0.99 * __enough || __result > 1.2 * __enough) { \
                if (__result > 150.) {                                     \
                    double tmp = __iterations / __result;                  \
                    tmp *= 1.1 * __enough;                                 \
                    __iterations = (iter_t)(tmp + 1);                      \
                } else {                                                   \
                    if (__iterations > (iter_t)1 << 27) {                  \
                        __result = 0.;                                     \
                        break;                                             \
                    }                                                      \
                    __iterations <<= 3;                                    \
                }                                                          \
            }                                                              \
        } /* while */                                                      \
        save_n((uint64)__iterations);                                      \
        settime((uint64)__result);                                         \
    }


typedef u_long iter_t;
typedef void (*benchmp_f)(iter_t iterations, void *cookie);

extern void   benchmp(benchmp_f initialize, benchmp_f benchmark, benchmp_f cleanup, int enough, int parallel, int warmup,
                      int repetitions, void *cookie);

/*
 * These are used by weird benchmarks which cannot return, such as page
 * protection fault handling.  See lat_sig.c for sample usage.
 */
extern void  *benchmp_getstate();
extern iter_t benchmp_interval(void *_state);

/*
 * Which child process is this?
 * Returns a number in the range [0, ..., N-1], where N is the
 * total number of children (parallelism)
 */
extern int    benchmp_childid();

/*
 * harvest dead children to prevent zombies
 */
extern void   sigchld_wait_handler(int signo);

/*
 * Handle optional pinning/placement of processes on an SMP machine.
 */
extern int    handle_scheduler(int childno, int benchproc, int nbenchprocs);

/*
 * Generated from msg.x which is included here:

    program XACT_PROG {
        version XACT_VERS {
        char
        RPC_XACT(char) = 1;
        } = 1;
    } = 3970;

 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#define XACT_PROG ((u_long)404040)
#define XACT_VERS ((u_long)1)
#define RPC_XACT  ((u_long)1)
#define RPC_EXIT  ((u_long)2)
extern char *rpc_xact_1();
extern char *client_rpc_xact_1();

#endif /* _BENCH_H */
