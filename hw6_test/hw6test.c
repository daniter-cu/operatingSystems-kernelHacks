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



/* running a number of memory accesses by setting variables and struct
 * values */
void *runThread1(void *arg)
{
    
    return(void *)0;
}


/* Stress test.  Create  many threads that all do memory accesses" */

long test1a()
{

	return 0;

}

/*
 * This is a regular test with two threads that do many memory
 * accesses.
 */
long test2a()
{
 

    return 0;
}


/*
 * This test to see that correct errors are returned on invalid
 * arguments.
 */
long test3a()
{

    return 0;
}


/*
 * Stress test.  See that tracing works after forks.
 */
long test4a()
{
 
    return 0;
}




struct testcase testcase1a = {"1a", "Stress test on many threads doing memory access.", test1a};
struct testcase testcase2a = {"2a", "Regular use of new system calls.  General Test", test2a};
struct testcase testcase3a = {"3a", "Test return of system calls when invalid args are passed.", test3a};
struct testcase testcase4a = {"4a", "Stress test for many processeses making threads that access memory pages.", test4a};


struct testcase **testcase;

void init_testcase() 
{
    int i = 0;

    testcase = malloc(sizeof(struct testcase) * 5);

    testcase[i++] = &testcase1a;
    testcase[i++] = &testcase2a;
    testcase[i++] = &testcase3a;
    testcase[i++] = &testcase4a;
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
	if(fd1 = open("tempdir/dir1/f1", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM) < 0) {
		fprintf(stderr, "Error creating file tempdir/dir1/f1: %s\n", strerror(errno));
	}
	if(close(fd1)) {
		fprintf(stderr, "Error closing newly-created file tempdir/dir1/f1: %s\n", strerror(errno));
	}
	if(fd2 = open("tempdir/dir1/f2", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM) < 0) {
		fprintf(stderr, "Error creating file tempdir/dir1/f2: %s\n", strerror(errno));
	}
	if(close(fd2)) {
		fprintf(stderr, "Error closing newly-created file tempdir/dir1/f2: %s\n", strerror(errno));
	}
	
}

void remove_directories() {
	//rm -r tempdir/
}
