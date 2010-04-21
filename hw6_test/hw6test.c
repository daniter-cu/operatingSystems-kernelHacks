#define _REENTRANT 
#define _GNU_SOURCE 

#include <math.h>   
#include <stdio.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <string.h> 
#include <unistd.h> 
#include <asm-i386/unistd.h>
#include <sys/wait.h> 
#include <sys/time.h> 
#include "test.h"
   
_syscall2(long, start_trace, unsigned long, start, size_t, size); 
_syscall0(long, stop_trace); 
_syscall2(long, get_trace, pid_t, tid, int *, wcounts);   
_syscall0(long, gettid);




/* running a number of memory accesses by setting variables and struct
 * values */
void *runThread1(void *arg)
{
    int x;
    int y;
    int i;
    int j;
    struct foo *temp;
    char *string;


    int *wcounts = (int *) malloc(sizeof(int)*5000);
    if(wcounts == NULL)
    {	
	printf("Malloc failed.\n");
	return  (void *) -1;
    }

    memset(wcounts, '\0', 5000);

    x = 5; 
    y = 6;
    string = "hello, world!\n";

    if (start_trace(pow(2,20), pow(2, 24) ) == -1) { 
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return (void *) -1;
    }

    for (i = 0; i < 50; i++)
    {
	x = x + y;
    	y = y - i;
    }

    for(j = 0; j < 50; j++)
    {
    	temp->id = 5;
    	temp->str = string;
    }

   if (stop_trace() < 0)
    {
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return (void*) -1;
    }
  
    if (get_trace(gettid(), wcounts) < 0)
	fprintf(stderr, "%s: get_trace failed\n", strerror(errno));

	
    for(i = 0; i < 5000; i++)
    {
	printf("%d", wcounts[i]);
    }
    printf("\n");


    free(wcounts);
    return(void *)0;
}


/* Stress test.  Create  many threads that all do memory accesses" */

long test1a()
{
    pthread_t t[100];

    int i;
    for(i = 0; i< 100; i++)
    {
  	if (pthread_create(&t[i], NULL, runThread1, NULL) == -1) {
		fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
         }
    }
    return 0;

}

/*
 * This is a regular test with two threads that do many memory
 * accesses.
 */
long test2a()
{
    pthread_t t1, t2;
  
    if (pthread_create(&t1, NULL, runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }
    if (pthread_create(&t2, NULL, runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    return 0;
}


/*
 * This test to see that correct errors are returned on invalid
 * arguments.
 */
long test3a()
{

    if (stop_trace()>=0)
    {
	fprintf(stderr, "SHOULD NOT WORK - stop_trace called before start_trace\n");
	return -1;
    }
    if (start_trace(-1, pow(2, 20) + pow(2, 24) ) >= 0) { 
	fprintf(stderr, "%s\n", "start_trace failed - given a negative start value");
	return -1;
    }

    if (start_trace(pow(2,20), -1 ) >= 0) { 
	fprintf(stderr, "%s\n", "start_trace failed - given a negative size value");
	return -1;
    }

    int wcounts[5000];

    if( get_trace(-4, wcounts) >= 0 )
    {
	printf("get_trace should not return success on invalid pid.\n");
	return -1;
    }

    free(wcounts);
    return 0;
}


/*
 * Stress test.  See that tracing works after forks.
 */
long test4a()
{
    int i;
    for(i = 0; i < 10; i++)
    {
	if(fork() < 0)
	    return -1;
	else
	{
   	 pthread_t t1, t2;
  
 	   if (pthread_create(&t1, NULL, runThread1, NULL) == -1)
    	{
		fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
		return -1;
    	}
    	if (pthread_create(&t2, NULL, runThread1, NULL) == -1)
    	{
		fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
		return -1;
	}
	}
    }
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
