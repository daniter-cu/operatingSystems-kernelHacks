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
   
long _syscall2(long, start_trace, unsigned long, start, size_t, size); 
long _syscall0(long, stop_trace); 
long _syscall2(long, get_trace, pid_t, tid, int *, wcounts);   
long _syscall0(long, gettid);




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

    temp = (struct foo*)malloc(sizeof(struct foo));
    if(temp == NULL)
    {
	printf("Malloc failed\n");
	return (void*) -1;
    }

    int *wcounts;
    wcounts = (int *) arg;

    x = 5; 
    y = 6;
    string = "hello, world!\n";

    if (start_trace(pow(2,20), pow(2, 20) + pow(2, 24) ) == -1) { //dunno about this - set it to the entire region
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return -1;
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
	return -1;
    }
  
    if (get_trace(gettid(), wcounts) < 0)
	fprintf(stderr, "%s: get_trace failed\n", strerror(errno));
}

/* running a number of memory accesses by setting variables and struct
 * values but for loop is only completed 5 times */
/*static int runThread2(void *arg)
{
    int x;
    int y;
    int i;
   // struct foo *temp;
    char *string;

    int *wcounts;
    wcounts = (int *) arg;

    x = 1; 
    y = 3;
    string = "hello, world!\n";

    for (i = 0; i < 5; i++)
    {
	x = x + y;
    }

    temp->id = 5;
    temp->str = string;

    if (get_trace(gettid(), wcounts) < 0)
	fprintf(stderr, "%s: get_trace failed\n", strerror(errno));
}
*/

/* two calls to sys_start_trace without sys_stop_trace inbetween */
/* should return -EINVAL */
long test1a()
{
    pthread_t t1, t2, t3;
    int *wcounts = (int *) malloc(sizeof(int)*5000);
    wcounts[4999] = '\0';

    if (pthread_create(&t1, NULL, runThread1, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    if (pthread_create(&t2, NULL, runThread1, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    if (pthread_create(&t3, NULL, runThread1, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    
    free(wcounts);
    return 0;

}

long test2a()
{
    pthread_t t1, t2;
    int *wcounts = (int *) malloc(sizeof(int)*5000);
    wcounts[4999] = '\0';


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

    stop_trace();
    free(wcounts);
    return 0;
}

long test3a()
{
    pthread_t t1, t2;
    int *wcounts = (int *) malloc(sizeof(int)*5000);
    wcounts[4999] = '\0';

    if (stop_trace()>=0)
    {
	fprintf(stderr, "SHOULD NOT WORK - stop_trace called before start_trace\n");
	return -1;
    }
    if (start_trace(-1, pow(2, 20) + pow(2, 24) ) >= 0) { 
	fprintf(stderr, "%s: start_trace failed - given a negative start value\n");
	return -1;
    }

    if (start_trace(pow(2,20), -1 ) >= 0) { //dunno about this - set it to the entire region
	fprintf(stderr, "%s: start_trace failed - given a negative size value\n");
	return -1;
    }

    if( get_trace(-4, wcounts) >= 0 )
    {
	printf("get_trace should not return success on invalid pid.\n");
	return -1;
    }

    free(wcounts);
    return 0;
}



struct testcase testcase1a = {"1a", "example in the homework - thread 2 should run roughly twice as long as thread 1 and thread 2", test1a};
struct testcase testcase2a = {"2a", "two threads in which the probability between the two colors is 10", test2a};
//struct testcase testcase2b = {"2b", "two threads in which the probability between the two colors is 0", test2b};
//struct testcase testcase3a = {"3a", "set the probability between two threads to an invalid value of -1", test3a};
//struct testcase testcase3b = {"3b", "set the color of the second thread to an invalid value of 9", test3b};
//struct testcase testcase4a = {"4a", "create five threads of different colors with probabilities inbetween", test4a};
//struct testcase testcase4b = {"4b", "create five threads in which there are multiple threads with the same color - should round robin between those threads", test4b};


struct testcase **testcase;

void init_testcase() 
{
    int i = 0;

    testcase = malloc(sizeof(struct testcase) * 8);

    testcase[i++] = &testcase1a;
    testcase[i++] = &testcase2a;
   // testcase[i++] = &testcase2b;
   // testcase[i++] = &testcase3a;
   // testcase[i++] = &testcase3b;
   // testcase[i++] = &testcase4a;
  //  testcase[i++] = &testcase4b;
    testcase[i++] = NULL; 
}
