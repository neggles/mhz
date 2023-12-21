#pragma once

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#ifdef RUSAGE
void rusage(void);
#endif

void     timing(FILE *out);
void     start(struct timeval *tv);
uint64_t stop(struct timeval *begin, struct timeval *end);
uint64_t now(void);
double   Now(void);
uint64_t delta(void);
double   Delta(void);

void     save_n(uint64_t);
uint64_t get_n(void);
void     settime(uint64_t usecs);
void     bandwidth(uint64_t bytes, uint64_t times, int verbose);

void kb(uint64_t bytes);
void mb(uint64_t bytes);
void latency(uint64_t xfers, uint64_t size);
void context(uint64_t xfers);
void nano(char *s, uint64_t n);
void micro(char *s, uint64_t n);
void micromb(uint64_t mb, uint64_t n);
void milli(char *s, uint64_t n);
void ptime(uint64_t n);

uint64_t tvdelta(struct timeval *, struct timeval *);
void     tvsub(struct timeval *tdiff, struct timeval *t1, struct timeval *t0);
uint64_t usecs_spent(void);
double   timespent(void);
char    *p64(uint64_t big);
char    *p64sz(uint64_t big);

char     last(char *s);
uint64_t bytes(char *s);
void     use_int(int result);
void     use_pointer(void *result);
double   l_overhead(void);
uint64_t t_overhead(void);
int      get_enough(int);
size_t  *permutation(int max, int scale);

void morefds(void);
long bread(void *src, long count);
void touch(char *buf, int size);
int  cp(char *src, char *dst, mode_t mode);
