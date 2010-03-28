#define _REENTRANT
#define _GNU_SOURCE

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pwd.h>
#include <sched.h>

#include "test.h"
#include "test-ras.h"

#define MAX_LEN 1024

_syscall2(long, setcolor, int, pid, int, color);
_syscall1(long, getcolor, int, color);
_syscall3(long, setprob, int, color1, int, color2, int, prob);
_syscall2(long, getprob, int, color1, int, color2);

static int init_prob_matrix() {
	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (setprob(i, j, 0) < 0) {
				perror("setprob");
				return -1;
			}
		}
	}
	return 0;
}

long test1a() {
	int i, j, k;
	int cur_prob[5][5];
	memset(cur_prob, 0, sizeof cur_prob);
	if (init_prob_matrix() < 0)
		return -1;
	for (i = -50; i <= 50; i++) {
		for (j = -50; j <= 50; j++) {
			for (k = -50; k <= 50; k++) {
				int std_ret, std_errno, ret;
				if (i >= 0 && i < 5 && j >= 0 && j < 5 && k >= 0 && k <= 10) {
					std_ret = 0;
					std_errno = 0;
					cur_prob[i][j] = cur_prob[j][i] = k;
				} else {
					std_ret = -1;
					std_errno = EINVAL;
				}
				errno = 0;
				ret = setprob(i, j, k);
				if (ret != std_ret || errno != std_errno) {
					return -1;
				}
				if (i >= 0 && i < 5 && j >= 0 && j < 5) {
					std_ret = cur_prob[i][j];
					std_errno = 0;
				} else {
					std_ret = -1;
					std_errno = EINVAL;
				}
				errno = 0;
				ret = getprob(i, j);
				if (ret != std_ret || errno != std_errno) {
					fprintf(stderr, "ret = %d, errno = %d\n", ret, errno);
					fprintf(stderr, "%d %d %d\n", i, j, k);
					return -1;
				}
			}
		}
	}
	return 0;
}

long test1b() {
	int k;
	int r;

	if (run_as_ras() < 0)
		return -1;

	for (k = -100; k <= 100; k++) {
		int ret = setcolor(0, k);
		if (k >= 0 && k < 5) {
			if (ret != 0)
				return -1;
		} else {
			if (ret != -1 || errno != EINVAL)
				return -1;
		}
	}

	r = setcolor(-1, 0);
	if (r != -1 || errno != ESRCH)
		return -1;

	return 0;
}

long test1c() {
	int ret;
	struct passwd *pw = getpwnam("student");
	
	if (run_as_ras() < 0)
		return -1;

	if (!pw) {
		perror("Cannot find the UID of 'student'");
		return -1;
	}
	ret = setuid(pw->pw_uid);
	if (ret < 0) {
		perror("setuid");
		return -1;
	}
	ret = setcolor(0, 0);
	if (ret != -1 || errno != EPERM) {
		fprintf(stderr, "ret = %d, errno = %d\n", ret, errno);
		return -1;
	}
	ret = setprob(0, 0, 0);
	if (ret != -1 || errno != EPERM)
		return -1;
	return 0;
}

static int busy() {
	int i;
	clock_t start = clock();
	for (i = 0; ; i++) {
		sched_yield();
		if (clock() - start >= CLOCKS_PER_SEC)
			break;
	}
	return 0;
}

static int child_main(int i) {
	struct timeval end;
	char file_name[MAX_LEN];
	FILE *fout;

	if (busy() < 0)
		return -1;
	if (gettimeofday(&end, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}
	snprintf(file_name, MAX_LEN, "/tmp/child.%d", i);
	fout = fopen(file_name, "w");
	if (!fout) {
		perror("fopen");
		return -1;
	}
	fprintf(fout, "%lu %lu\n", end.tv_sec, end.tv_usec);
	fclose(fout);
	return 0;
}

long test2a() {
	/*
	 * CPU0: Child 0 Color 0
	 * CPU1: Child 1 Color 0
	 * CPU0 or CPU1: Parent Color 1
	 */
	int ret;
	int i;
	int status;
	struct timeval start, end_child[2], end, diff;
	long double self, child_0, child_1;

	if (init_prob_matrix() < 0)
		return -1;

	if (setprob(1, 0, 10) < 0) {
		perror("setprob");
		return -1;
	}

	if (run_as_ras() < 0)
		return -1;
	if (set_affinity(1 + 2) < 0)
		return -1;
	if (setcolor(0, 1) < 0) {
		perror("setcolor");
		return -1;
	}

	if (gettimeofday(&start, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}

	for (i = 0; i < 2; i++) {
		ret = fork();
		if (ret < 0) {
			perror("fork");
			return -1;
		}
		if (ret == 0) {
			if (set_affinity(1 << i) < 0)
				exit(-1);
			if (setcolor(0, 0) < 0) {
				perror("setcolor");
				exit(-1);
			}
			int ret_child = child_main(i);
			exit(ret_child);
		}
	}
	
	if (busy() < 0)
		return -1;
	if (gettimeofday(&end, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}

	for (i = 0; i < 2; i++) {
		if (wait(&status) < 0) {
			perror("wait");
			return -1;
		}
		if (status != 0)
			return -1;
	}

	for (i = 0; i < 2; i++) {
		char file_name[MAX_LEN];
		FILE *fin;
		snprintf(file_name, MAX_LEN, "/tmp/child.%d", i);
		fin = fopen(file_name, "r");
		if (!fin) {
			perror("fopen");
			return -1;
		}
		fscanf(fin, "%lu %lu", &end_child[i].tv_sec, &end_child[i].tv_usec);
		fclose(fin);
	}

	timersub(&end_child[0], &start, &diff);
	child_0 = timeval_to_sec(&diff);
	timersub(&end_child[1], &start, &diff);
	child_1 = timeval_to_sec(&diff);
	timersub(&end, &start, &diff);
	self = timeval_to_sec(&diff);
	fprintf(stderr, "Running time (sec):\n");
	fprintf(stderr, "\tParent: %Lf\n\tChild 0: %Lf\n\tChild 1: %Lf\n", self, child_0, child_1);

	if (self / child_0 < 1.8 || self / child_0 > 2.2)
		return -1;
	if (self / child_1 < 1.8 || self / child_1 > 2.2)
		return -1;

	return 0;
}

long test2b() {
	struct timeval start, diff, end_child[3];
	long double time_rt, time_ras, time_normal;
	int i;

	if (run_as_rt() < 0)
		return -1;
	if (gettimeofday(&start, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}
	if (set_affinity(1) < 0)
		return -1;

	for (i = 0; i < 3; i++) {
		int ret = fork();
		if (ret < 0) {
			perror("fork");
			return -1;
		}
		if (ret == 0) {
			if (i == 0) {
				if (run_as_normal() < 0)
					exit(-1);
			} else if (i == 1) {
				if (run_as_ras() < 0)
					exit(-1);
			} else if (i == 2) {
				if (run_as_rt() < 0)
					exit(-1);
			}
			int ret_child = child_main(i);
			exit(ret_child);
		}
	}

	for (i = 0; i < 3; i++) {
		int status;
		if (wait(&status) < 0) {
			perror("wait");
			return -1;
		}
		if (status != 0)
			return -1;
	}

	for (i = 0; i < 3; i++) {
		char file_name[MAX_LEN];
		FILE *fin;
		snprintf(file_name, MAX_LEN, "/tmp/child.%d", i);
		fin = fopen(file_name, "r");
		if (!fin) {
			perror("fopen");
			return -1;
		}
		fscanf(fin, "%lu %lu", &end_child[i].tv_sec, &end_child[i].tv_usec);
		fclose(fin);
	}

	timersub(&end_child[0], &start, &diff);
	time_normal = timeval_to_sec(&diff);
	timersub(&end_child[1], &start, &diff);
	time_ras = timeval_to_sec(&diff);
	timersub(&end_child[2], &start, &diff);
	time_rt = timeval_to_sec(&diff);

	fprintf(stderr, "RT = %Lf\nRAS = %Lf\nNormal = %Lf\n", time_rt, time_ras, time_normal);

	if (time_ras / time_rt < 1.8 || time_ras / time_rt > 2.2)
		return -1;
	if (time_normal / time_rt < 2.8 || time_normal / time_rt > 3.2)
		return -1;
	
	return 0;
}

#define N_CHILDREN 3

long test2c() {
	/*
	 * CPU0: A task of Color 0
	 * CPU1: Several tasks of Color 1
	 */
	int ret, ph;
	int i;
	int status;
	struct timeval start, end_child[N_CHILDREN];
	long double child[N_CHILDREN];

	if (init_prob_matrix() < 0)
		return -1;

	if (setprob(1, 0, 10) < 0) {
		perror("setprob");
		return -1;
	}

	if (run_as_ras() < 0)
		return -1;
	if (set_affinity(1) < 0)
		return -1;
	if (setcolor(0, 0) < 0) {
		perror("setcolor");
		return -1;
	}

	if (gettimeofday(&start, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}

	ret = fork();
	if (ret < 0) {
		perror("fork");
		return -1;
	}
	if (ret == 0) {
		/*
		 * Take up CPU0 for a sufficiently long time
		 * so that its chidren on the other CPU can always see it running.
		 */
		while (1)
			sched_yield();
		exit(0);
	}
	ph = ret;
	for (i = 0; i < N_CHILDREN; i++) {
		ret = fork();
		if (ret < 0) {
			perror("fork");
			return -1;
		}
		if (ret == 0) {
			if (set_affinity(2) < 0)
				exit(-1);
			if (setcolor(0, 1) < 0) {
				perror("setcolor");
				exit(-1);
			}
			int ret_child = child_main(i);
			exit(ret_child);
		}
	}
	
	for (i = 0; i < N_CHILDREN; i++) {
		if (wait(&status) < 0) {
			perror("wait");
			return -1;
		}
		if (status != 0)
			return -1;
	}
	if (kill(ph, SIGINT) < 0) {
		perror("kill");
		return -1;
	}
	if (wait(&status) < 0) {
		perror("wait");
		return -1;
	}
	if (WTERMSIG(status) != SIGINT) {
		fprintf(stderr, "Incorrect status of the child process %d\n", ph);
		return -1;
	}

	for (i = 0; i < N_CHILDREN; i++) {
		char file_name[MAX_LEN];
		FILE *fin;
		snprintf(file_name, MAX_LEN, "/tmp/child.%d", i);
		fin = fopen(file_name, "r");
		if (!fin) {
			perror("fopen");
			return -1;
		}
		fscanf(fin, "%lu %lu", &end_child[i].tv_sec, &end_child[i].tv_usec);
		fclose(fin);
	}

	for (i = 0; i < N_CHILDREN; i++) {
		struct timeval diff;
		timersub(&end_child[i], &start, &diff);
		child[i] = timeval_to_sec(&diff);
	}
	fprintf(stderr, "Running time (sec):\n");
	for (i = 0; i < N_CHILDREN; i++)
		fprintf(stderr, "\tChild %d: %Lf\n", i, child[i]);

	for (i = 0; i < N_CHILDREN; i++) {
		if (child[i] / child[0] < 0.8 || child[i] / child[0] > 1.2)
			return -1;
	}
	return 0;
}

long test2d() {
	/*
	 * CPU0: A task of Color 0
	 * CPU1: Several tasks of Color 1, 2, ...
	 */
	int ret, ph;
	int i;
	int status;
	struct timeval start, end_child[N_CHILDREN];
	long double child[N_CHILDREN];

	if (init_prob_matrix() < 0)
		return -1;

	for (i = 1; i < 5; i++) {
		if (setprob(i, 0, 10) < 0) {
			perror("setprob");
			return -1;
		}
	}

	if (run_as_ras() < 0)
		return -1;
	if (set_affinity(1) < 0)
		return -1;
	if (setcolor(0, 0) < 0) {
		perror("setcolor");
		return -1;
	}

	if (gettimeofday(&start, NULL) < 0) {
		perror("gettimeofday");
		return -1;
	}

	ret = fork();
	if (ret < 0) {
		perror("fork");
		return -1;
	}
	if (ret == 0) {
		/*
		 * Take up CPU0 for a sufficiently long time
		 * so that its chidren on the other CPU can always see it running.
		 */
		while (1)
			sched_yield();
		exit(0);
	}
	ph = ret;
	for (i = 0; i < N_CHILDREN; i++) {
		ret = fork();
		if (ret < 0) {
			perror("fork");
			return -1;
		}
		if (ret == 0) {
			if (set_affinity(2) < 0)
				exit(-1);
			if (setcolor(0, i + 1) < 0) {
				perror("setcolor");
				exit(-1);
			}
			int ret_child = child_main(i);
			exit(ret_child);
		}
	}
	
	for (i = 0; i < N_CHILDREN; i++) {
		if (wait(&status) < 0) {
			perror("wait");
			return -1;
		}
		if (status != 0)
			return -1;
	}
	if (kill(ph, SIGINT) < 0) {
		perror("kill");
		return -1;
	}
	if (wait(&status) < 0) {
		perror("wait");
		return -1;
	}
	if (WTERMSIG(status) != SIGINT) {
		fprintf(stderr, "Incorrect status of the child process %d\n", ph);
		return -1;
	}

	for (i = 0; i < N_CHILDREN; i++) {
		char file_name[MAX_LEN];
		FILE *fin;
		snprintf(file_name, MAX_LEN, "/tmp/child.%d", i);
		fin = fopen(file_name, "r");
		if (!fin) {
			perror("fopen");
			return -1;
		}
		fscanf(fin, "%lu %lu", &end_child[i].tv_sec, &end_child[i].tv_usec);
		fclose(fin);
	}

	for (i = 0; i < N_CHILDREN; i++) {
		struct timeval diff;
		timersub(&end_child[i], &start, &diff);
		child[i] = timeval_to_sec(&diff);
	}
	fprintf(stderr, "Running time (sec):\n");
	for (i = 0; i < N_CHILDREN; i++)
		fprintf(stderr, "\tChild %d: %Lf\n", i, child[i]);

	for (i = 0; i < N_CHILDREN; i++) {
		if (child[i] / child[0] < 0.8 || child[i] / child[0] > 1.2)
			return -1;
	}
	return 0;
}

#define MAX_N_CHILDREN 20

void *thread_run(void *arg) {
	int i, j;
	for (i = 0; i < 10000; i++) {
		for (j = 0; j < 50; j++)
			sched_yield();
	}
	return NULL;
}

long test3a() {
	pthread_t children[MAX_N_CHILDREN];
	int i;

	if (run_as_ras() < 0)
		return -1;

	for (i = 0; i < MAX_N_CHILDREN; i++) {
		fprintf(stderr, "Creating Thread %d\n", i);
		if (pthread_create(&children[i], NULL, thread_run, NULL) < 0) {
			perror("pthread_create");
			return -1;
		}
	}
	if (thread_run(NULL) < 0)
		return -1;
	for (i = 0; i < MAX_N_CHILDREN; i++) {
		void *status;
		if (pthread_join(children[i], &status) < 0) {
			perror("pthread_join");
			return -1;
		}
		if (status != NULL)
			return -1;
	}
	return 0;
}

struct testcase testcase1a = {
	"1a",
	"Invalid arguments for setprob/getprob",
	test1a
};

struct testcase testcase1b = {
	"1b",
	"Invalid arguments for setcolor/getcolor",
	test1b
};

struct testcase testcase1c = {
	"1c",
	"Non-root users don't have privileges to change color/prob",
	test1c
};

struct testcase testcase2a = {
	"2a",
	"Three tasks.\n"
		"Two of them like each other and hate the other task.",
	test2a
};

struct testcase testcase2b = {
	"2b",
	"RT > RAS > Normal",
	test2b
};

struct testcase testcase2c = {
	"2c",
	"A task of Color 0 takes up one CPU.\n"
		"Several tasks of Color 1 run on the other CPU.\n"
		"No starvation should occur.",
	test2c
};

struct testcase testcase2d = {
	"2d",
	"A task of Color 0 takes up one CPU.\n"
		"Several tasks of different colors run on the other CPU.\n"
		"These colors have the same race probability.\n"
		"No starvation should occur.",
	test2d
};

struct testcase testcase3a = {
	"3a",
	"Stress testing. A bunch of threads.\n"
		"Takes around half a minute on my machine.",
	test3a
};

struct testcase **testcase;

#define TESTNUM 8

void init_testcase() {
	int i = 0;

	testcase = malloc(sizeof(struct testcase) * (TESTNUM + 1));

	testcase[i++] = &testcase1a;
	testcase[i++] = &testcase1b;
	testcase[i++] = &testcase1c;
	testcase[i++] = &testcase2a;
	testcase[i++] = &testcase2b;
	testcase[i++] = &testcase2c;
	testcase[i++] = &testcase2d;
	testcase[i++] = &testcase3a;
	testcase[i++] = NULL;
}
