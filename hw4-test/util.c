#define _REENTRANT
#define _GNU_SOURCE

#include "test-ras.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>

int run_as_ras() {
	struct sched_param param;
	param.sched_priority = 0;
	if (sched_setscheduler(0, SCHED_RAS, &param) < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return 0;
}

int run_as_rt() {
	struct sched_param param;
	param.sched_priority = 1; // the lowest rt priority
	if (sched_setscheduler(0, SCHED_RR, &param) < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return 0;
}

int run_as_normal() {
	struct sched_param param;
	param.sched_priority = 0;
	if (sched_setscheduler(0, SCHED_NORMAL, &param) < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return 0;
}

int set_affinity(int mask) {
	cpu_set_t set;
	CPU_ZERO(&set);
	if (mask & 1)
		CPU_SET(0, &set);
	if (mask & 2)
		CPU_SET(1, &set);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0) {
		perror("sched_setaffinity");
		return -1;
	}
	return 0;
}

long double timeval_to_sec(const struct timeval *t) {
	return t->tv_sec + (long double)t->tv_usec / 1000000;
}
