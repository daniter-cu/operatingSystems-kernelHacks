/*---Start of C file------*/
#define _REENTRANT
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "test.h"
#include "test-pagetrace.h"


#define PAGE_NUM 10
#define PAGE_SIZE (getpagesize())
#define ADDR_START ((void *)0x01000000)

long test1a() {
	unsigned char *pages;
	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size * PAGE_NUM;
	unsigned long addr;
	int wcount[PAGE_NUM], i, ret;

	bzero(wcount, PAGE_NUM * sizeof(int));

	pages = mmap(ADDR_START,
		addr_size,	
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0,
		0);
	if (pages == MAP_FAILED)
		return -errno;

	fprintf(stderr, "pages = %lx\n", (unsigned long)pages);


	for (i = 0 ; i < 1000 ; i++) {
		for (addr = 0 ; addr < addr_size ; addr += page_size)
			pages[addr] = 0;
		usleep(1);
	}
	fprintf(stderr, "write each page 1000 times\n");

	ret = start_trace((unsigned long)pages, addr_size);
	if (ret) {
		fprintf(stderr, "start_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "start_trace\n");

	for (i = 0 ; i < 1000 ; i++) {
		for (addr = 0 ; addr < addr_size ; addr += page_size)
			pages[addr] = 0;
		usleep(1);
	}
	fprintf(stderr, "write each page 1000 times\n");

	ret = get_trace(getpid(), wcount);
	if (ret) {
		fprintf(stderr, "get_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "get_trace\n");
	
	for (i = 0 ; i < PAGE_NUM ; i++)
		printf("wcount[%d] = %d\n", i, wcount[i]);
	
	ret = stop_trace();
	if (ret) {
		fprintf(stderr, "stop_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "stop_trace\n");

	for (i = 0 ; i < PAGE_NUM ; i++)
		if (wcount[i] < 900)
			return -1;

	return 0;
}
struct testcase testcase1a = {"1a", "easy test", test1a};

void *thread1b(void *args) {
	unsigned char *pages = (unsigned char *)((void **)args)[0];
	int *wcount = (int *)((void **)args)[1];

	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size * PAGE_NUM;
	unsigned long addr;
	int ret, i;

	fprintf(stderr, "in child: new thread created\n");

	for (i = 0 ; i < 10 ; i++) {
		for (addr = 0 ; addr < addr_size ; addr += page_size)
			pages[addr]++;
		usleep(20);
	}
	
	ret = get_trace(gettid(), wcount);
	if (ret) {
		fprintf(stderr, "in child: get_trace failed: %s\n", 
				strerror(errno));
		return NULL;
	}
	fprintf(stderr, "in child: get_trace\n");
	
	return NULL;
}

long test1b() {
	unsigned char *pages;
	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size * PAGE_NUM;
	
	unsigned long addr;
	int wcount[PAGE_NUM], wcount2[PAGE_NUM], i, ret;
	void *args[2];

	pthread_t thread;

	bzero(wcount, PAGE_NUM * sizeof(int));
	bzero(wcount2, PAGE_NUM * sizeof(int));
	
	pages = mmap(ADDR_START,
		addr_size,	
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0,
		0);
	if (pages == MAP_FAILED)
		return -errno;
	fprintf(stderr, "pages = %lx\n", (unsigned long)pages);

	bzero(pages, addr_size);

	ret = start_trace((unsigned long)pages, addr_size);
	if (ret) {
		fprintf(stderr, "start_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "start_trace\n");

	args[0] = pages;
	args[1] = wcount2;
	pthread_create(&thread, NULL, thread1b, args);
	pthread_join(thread, NULL);

	usleep(10);
	
	for (i = 0 ; i < 10 ; i++) {
		for (addr = 0 ; addr < addr_size ; addr += page_size)
			pages[addr]++;
		usleep(20);
	}
	ret = get_trace(gettid(), wcount);
	if (ret) {
		fprintf(stderr, "get_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "get_trace\n");

	for (i = 0 ; i < PAGE_NUM ; i++)
		printf("wcount[%d]=%02d, wcount2[%d]=%02d, page[%04X]=%d\n",
			i, wcount[i], i, wcount2[i], 
			i * page_size, pages[i * page_size]);

	ret = stop_trace();
	if (ret) {
		fprintf(stderr, "stop_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "stop_trace\n");

	for (i = 0 ; i < PAGE_NUM ; i++)
		if (wcount[i] < 9  || wcount[i] > 10 || 
		    wcount2[i] < 9 || wcount[i] > 10)
			return -1;

	return 0;
}
struct testcase testcase1b = {"1b", "multithread test", test1b};

long test1c() {
	unsigned char *pages;
	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size * PAGE_NUM;
	int wcount[PAGE_NUM], ret;

	bzero(wcount, PAGE_NUM * sizeof(int));

	pages = mmap(ADDR_START,
		addr_size,	
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0,
		0);
	if (pages == MAP_FAILED)
		return -errno;

	fprintf(stderr, "pages = %lx\n", (unsigned long)pages);

	ret = start_trace((unsigned long)pages, addr_size);
	if (ret) {
		fprintf(stderr, "start_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "start_trace\n");

	ret = start_trace((unsigned long)pages, addr_size);
	if (!ret || errno != EINVAL) {
		fprintf(stderr, "the 2nd start_trace should fail\n");
		return -1;
	}

	ret = get_trace(getpid(), wcount);
	if (ret) {
		fprintf(stderr, "get_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "get_trace\n");
	
	ret = stop_trace();
	if (ret) {
		fprintf(stderr, "stop_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "stop_trace\n");

	ret = stop_trace();
	if (!ret || errno != EINVAL) {
		fprintf(stderr, "the 2nd stop_trace should fail\n");
		return -1;
	}

	ret = get_trace(getpid(), wcount);
	if (!ret || errno != EINVAL) {
		fprintf(stderr, "the 2nd get_trace should fail\n");
		return -1;
	}
	
	return 0;
}
struct testcase testcase1c = {"1c", "error test", test1c};

long test2a() {
	unsigned char *pages;
	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size * PAGE_NUM;
	int wcount[PAGE_NUM], ret, i, stat;
	unsigned long addr;
	pid_t pid;

	bzero(wcount, PAGE_NUM * sizeof(int));

	pages = mmap(ADDR_START,
		addr_size,	
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0,
		0);
	if (pages == MAP_FAILED)
		return -errno;

	fprintf(stderr, "pages = %lx\n", (unsigned long)pages);

	ret = start_trace((unsigned long)pages, addr_size);
	if (ret) {
		fprintf(stderr, "start_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "start_trace\n");

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork failed: %s\n", strerror(errno));
		return -errno;
	}

	if (pid == 0) {
		for (i = 0 ; i < 10 ; i++) {
			for (addr = 0 ; addr < addr_size ; addr += page_size)
				pages[addr]++;
			usleep(20);
		}
	
		exit(0);
	}

	wait(&stat);
	for (i = 0 ; i < 10 ; i++) {
		for (addr = 0 ; addr < addr_size ; addr += page_size)
			pages[addr]++;
		usleep(20);
	}

	ret = get_trace(getpid(), wcount);
	if (ret) {
		fprintf(stderr, "get_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "get_trace\n");

	for (i = 0 ; i < PAGE_NUM ; i++)
		printf("wcount[%d] = %d, page[%04X] = %d\n", i, wcount[i], 
		       i * page_size, pages[i * page_size]);	

	ret = stop_trace();
	if (ret) {
		fprintf(stderr, "stop_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "stop_trace\n");

	for (i = 0 ; i < PAGE_NUM ; i++)
		if (pages[i * page_size] != 10)
			return -1;

	return 0;
}
struct testcase testcase2a = {"2a", "interfere with COW", test2a};

void *thread2b(void *args) {
	unsigned char *pages = (unsigned char *)((void **)args)[0];
	int *wcount = (int *)((void **)args)[1];

	int ret, i;

	fprintf(stderr, "in child: new thread created\n");

	for (i = 0 ; i < 10 ; i++) {
		while(!pages[0]);
		pages[0]--;
	}
	
	ret = get_trace(gettid(), wcount);
	if (ret) {
		fprintf(stderr, "in child: get_trace failed: %s\n", 
				strerror(errno));
		return NULL;
	}
	fprintf(stderr, "in child: get_trace\n");
	
	return NULL;
}

long test2b() {
	unsigned char *pages;
	size_t page_size = PAGE_SIZE;
	size_t addr_size = page_size;

	int wcount[1], wcount2[1], i, ret;
	void *args[2];

	pthread_t thread;

	bzero(wcount, sizeof(int));
	bzero(wcount2, sizeof(int));
	
	pages = mmap(ADDR_START,
		addr_size,	
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		0,
		0);
	if (pages == MAP_FAILED)
		return -errno;
	fprintf(stderr, "pages = %lx\n", (unsigned long)pages);

	bzero(pages, addr_size);

	ret = start_trace((unsigned long)pages, addr_size);
	if (ret) {
		fprintf(stderr, "start_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "start_trace\n");

	args[0] = pages;
	args[1] = wcount2;
	pthread_create(&thread, NULL, thread2b, args);
	usleep(1);
	
	for (i = 0 ; i < 10 ; i++) {
		pages[0]++;
		usleep(20);
	}

	pthread_join(thread, NULL);

	ret = get_trace(gettid(), wcount);
	if (ret) {
		fprintf(stderr, "get_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "get_trace\n");

	printf("wcount[0]=%02d, wcount2[0]=%02d, page[0]=%d\n",
		wcount[0], wcount2[0], pages[0]);

	ret = stop_trace();
	if (ret) {
		fprintf(stderr, "stop_trace failed: %s\n", strerror(errno));
		return -errno;
	}
	fprintf(stderr, "stop_trace\n");

	if (wcount[0] < 9 || wcount2[0] > 0)
		return -1;

	return 0;
}
struct testcase testcase2b = {"2b", "interesting test", test2b};

struct testcase **testcase;

#define TESTNUM 5

void init_testcase() {
	int i = 0;

	testcase = malloc(sizeof(struct testcase) * (TESTNUM + 1));

	testcase[i++] = &testcase1a;
	testcase[i++] = &testcase1b;
	testcase[i++] = &testcase1c;
	testcase[i++] = &testcase2a;
	testcase[i++] = &testcase2b;
	testcase[i++] = NULL;
}

