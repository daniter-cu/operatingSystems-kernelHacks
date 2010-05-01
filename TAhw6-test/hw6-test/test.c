#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "test.h"

struct testcase **testcase;
struct testcase *current_testcase;

#define ISVALIDTESTNUM(num) (isdigit(num[0]) && isalpha(num[1]))
int ISGOODTESTNUM(char *num) {
	int count;
	
	if (!ISVALIDTESTNUM(num))
		return 0;
	for (count = 0 ; testcase[count] != NULL ; count++) {
		if (strncmp(num, testcase[count]->num, 2) == 0)
			return 1;
	}
	return 0;
}

void runtest(struct testcase *test) {
	pid_t pid;

	current_testcase = test;

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "[main] testcase forking failed.\n");
		current_testcase = NULL;
		exit(-EFAULT);	
	}
	
	if (pid == 0) {
		fprintf(stderr, "[main] test case %s: %s\n",
			test->num, test->des);
		if (test->test() < 0) 
			fprintf(stderr, "[main] test case %s failed.\n",
					test->num);
		else
			fprintf(stderr, "[main] test case %s passed.\n",
					test->num);

		exit(0);
	}
	else
		waitpid(pid, NULL, 0);

	current_testcase = NULL;
}


