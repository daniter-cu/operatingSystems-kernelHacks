#define _REENTRANT 
#define _GNU_SOURCE 

#include <math.h>   
#include <stdio.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>
#include "test.h"
   
#define FULL_PERM (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)


void *runThread1(void *arg)
{
    /* pin the inodes */
    if (access("tempdir/dir1/f1", R_OK)!=0)
    {
	exit(1);
    }

    sleep(10);

    return(void *)0;
}

void *runThread2(void *arg)
{
    if ((remove("tempdir/dir1/f1"))<0)
    {
	fprintf(stderr, "%s: remove failed\n", strerror(errno));
	return (void*)-1;
    }

    return 0;
}

void *runThread3(void *arg)
{
    if ((remove("tempdir/"))<0)
    {
	fprintf(stderr, "%s: remove failed\n", strerror(errno));
	return (void*)-1;
    }

    return 0;
}


long test1a()
{
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    if (pthread_create(&t2, NULL, runThread2, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    return 0;
}


long test2a()
{
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    if (pthread_create(&t2, NULL, runThread3, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }


    return 0;
}


long test3a()
{
    pthread_t t1, t2;

    if ((link("tempdir/dir1/f1", "tempdir/dir1/f2")) < 0)
    {
	fprintf(stderr, "%s: link failed\n", strerror(errno));
	return -1;
    }

    if (pthread_create(&t1, NULL, runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    if (pthread_create(&t2, NULL, runThread2, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }
    return 0;
}


struct testcase testcase1a = {"1a", "For regular file, one thread accesses and pins while another tries to modify by remove", test1a};
struct testcase testcase2a = {"2a", "For regular file, one thread accesses and pins while another tries to modify the home", test2a};
struct testcase testcase3a = {"3a", "For a link, one thread accesses and pins while another tries to modify that link", test3a};


struct testcase **testcase;

void init_testcase() 
{
    int i = 0;

    testcase = malloc(sizeof(struct testcase) * 4);

    testcase[i++] = &testcase1a;
    testcase[i++] = &testcase2a;
    testcase[i++] = &testcase3a;
    testcase[i++] = NULL; 
}

void init_directories() {
	int fd1, fd2;
	if(mkdir("tempdir", FULL_PERM)) {
		fprintf(stderr, "Error creating tempdir: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(mkdir("tempdir/dir1", FULL_PERM)) {
		fprintf(stderr, "Error creating tempdir/dir1: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if((fd1 = open("tempdir/dir1/f1", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM)) < 0) {
		fprintf(stderr, "Error creating file tempdir/dir1/f1: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(close(fd1)) {
		fprintf(stderr, "Error closing newly-created file tempdir/dir1/f1: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if((fd2 = open("tempdir/dir1/f2", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM)) < 0) {
		fprintf(stderr, "Error creating file tempdir/dir1/f2: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(close(fd2)) {
		fprintf(stderr, "Error closing newly-created file tempdir/dir1/f2: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
}

void remove_directories() {
	if(remove("tempdir/dir1/f1")) {
		fprintf(stderr, "Error removing tempdir/dir1/f1: %s\n", strerror(errno));
	}
	if(remove("tempdir/dir1/f2")) {
		fprintf(stderr, "Error removing tempdir/dir1/f2: %s\n", strerror(errno));
	}
	if(remove("tempdir/dir1/")) {
		fprintf(stderr, "Error removing tempdir/dir1/: %s\n", strerror(errno));
	}
	if(remove("tempdir/")) {
		fprintf(stderr, "Error removing tempdir/: %s\n", strerror(errno));
	}
}
