#ifndef __TEST_RAS_H
#define __TEST_RAS_H

#include <linux/unistd.h>
#include <sys/time.h>
#include <time.h>

#define SCHED_NORMAL 0
#define SCHED_RAS 4

int run_as_ras();
int run_as_rt();
int run_as_normal();
int set_affinity(int mask);
long double timeval_to_sec(const struct timeval *t);

#endif

