#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

/* running a number of memory accesses by setting variables and struct
 * values */
static int runThread1(void *arg)
{
    int x;
    int y;
    int i;
    struct foo *temp;
    char *string;

    int *wcounts;
    wcounts = (int *) arg;

    x = 5; 
    y = 6;
    string = "hello, world!\n";

    for (i = 0; i < 50; i++)
    {
	x = x + y;
    	y = y - i;
    }

    temp->id = 5;
    temp->str = string;
   
    if (get_trace(gettid(), wcounts) < 0)
	fprintf(stderr, "%s: get_trace failed\n", strerror(errno));
}

/* running a number of memory accesses by setting variables and struct
 * values but for loop is only completed 5 times */
static int runThread2(void *arg)
{
    int x;
    int y;
    int i;
    struct foo *temp;
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


/* two calls to sys_start_trace without sys_stop_trace inbetween */
/* should return -EINVAL */
long test1a()
{
    pthread_t t1, t2, t3;
    int *wcounts = (int *) malloc(sizeof(int)*5000);
    wcounts[4999] = '\0';

    if (start_trace(pow(2,20), pow(2, 20) + pow(2, 24) ) == -1) { //dunno about this - set it to the entire region
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return -1;
    }
    if (pthread_create(&t1, NULL, &runThread1, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    if (pthread_create(&t2, NULL, &runThread1, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    if (pthread_create(&t3, NULL, &runThread2, (void*) wcounts) == -1) {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
    }
    if (start_trace(pow(2,20), pow(2, 20) + pow(2, 24) ) == -1) { //dunno about this - set it to the entire region
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return -1;
    }
    free(wcounts);
    return 0;

}

long test2a()
{
    pthread_t t1, t2;
    int *wcounts = (int *) malloc(sizeof(int)*5000);
    wcounts[4999] = '\0';


    if (start_trace(pow(2,20), pow(2, 20) + pow(2, 24) ) == -1) { //dunno about this - set it to the entire region
	fprintf(stderr, "%s: start_trace failed\n", strerror(errno));
	return -1;
    }  	
    if (pthread_create(&t1, NULL, &runThread1, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }
    if (pthread_create(&t2, NULL, &runThread2, NULL) == -1)
    {
	fprintf(stderr, "%s: pthread_create failed\n", strerror(errno));
	return -1;
    }

    stop_trace();
    free(wcounts);
    return 0;
}



struct testcase testcase1a = {"1a", "example in the homework - thread 2 should run roughly twice as long as thread 1 and thread 2", test1a};
struct testcase testcase2a = {"2a", "two threads in which the probability between the two colors is 10", test2a};
struct testcase testcase2b = {"2b", "two threads in which the probability between the two colors is 0", test2b};
struct testcase testcase3a = {"3a", "set the probability between two threads to an invalid value of -1", test3a};
struct testcase testcase3b = {"3b", "set the color of the second thread to an invalid value of 9", test3b};
struct testcase testcase4a = {"4a", "create five threads of different colors with probabilities inbetween", test4a};
struct testcase testcase4b = {"4b", "create five threads in which there are multiple threads with the same color - should round robin between those threads", test4b};


struct testcase **testcase;

void init_testcase() 
{
    int i = 0;

    testcase = malloc(sizeof(struct testcase) * 8);

    testcase[i++] = &testcase1a;
    testcase[i++] = &testcase2a;
    testcase[i++] = &testcase2b;
    testcase[i++] = &testcase3a;
    testcase[i++] = &testcase3b;
    testcase[i++] = &testcase4a;
    testcase[i++] = &testcase4b;
    testcase[i++] = NULL; 
}
