#define _REENTRANT 
#define _GNU_SOURCE 

#include <math.h>   
#include <stdio.h> 
#include <errno.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "test.h"
   
#define FULL_PERM (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)



long test1a()
{
	init_directories();
    	int forkval;
	int status;
	char *args[] = {"-f", "tempdir/dir1/f2", (char *)0};



	if(access("tempdir/dir1/f2", F_OK)) {
		fprintf(stderr, "%s: could not access f2\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	forkval = fork();
	switch(forkval) {
	case -1: fprintf(stderr, "%s: fork fail\n", strerror(errno));
		exit(EXIT_FAILURE);
	case 0: 
		if(!(remove("tempdir/dir1/f2"))) {
			fprintf(stderr, "rm f2 should not have worked\n");
			remove_directories();
			exit(EXIT_FAILURE);		
		}
		else {
			exit(EXIT_SUCCESS);
		}
		break;
	default: 
		if(usleep(500000)) {
			fprintf(stderr, "%s: usleep failed\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		wait(&status);
		if(WEXITSTATUS(status)==EXIT_FAILURE) {
			fprintf(stderr, "exit failure detected\n");
			remove_directories();
			return -1;
		}
		break;
	}
	fprintf(stderr, "1a removing dirs\n");
	remove_directories();
    return 0;
}


long test2a()
{
	init_directories();
	int forkval;
	int status;
	char *args[] = {"", "tempdir/dir1/", "tempdir/dir2/", (char *)0};
	
	

	if(access("tempdir/dir1/", F_OK)) {
		fprintf(stderr, "%s: could not access dir1\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	forkval = fork();
	switch(forkval) {
	case -1: fprintf(stderr, "%s: fork fail\n", strerror(errno));
		exit(EXIT_FAILURE);
	case 0: 
		if((rename("tempdir/dir1/", "tempdir/dir2/"))) {
			perror("here");
			fprintf(stderr,  "mv dir1 should not have worked\n");
			remove_directories();
			exit(EXIT_FAILURE);
		
		}
		else {
			exit(EXIT_SUCCESS);
		}
		break;
	default: 
		if(usleep(500000)) {
			fprintf(stderr, "%s: usleep failed\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		wait(&status);
		if(WEXITSTATUS(status) == EXIT_FAILURE) {
			remove_directories();
			return -1;
		}
		break;
	}

	remove_directories();
    return 0;
}


long test3a() { 
	init_directories();
	int forkval;
	int status;
	char *args[] = {"", "-s", "tempdir/dir1/secretfile", "tempdir/dir1/sl1", (char *)0};



	if(access("tempdir/dir1/sl1", F_OK)) {
		fprintf(stderr, "%s: could not open symlink\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(remove("tempdir/dir1/sl1")) {
		fprintf(stderr, "%s: could not remove symlink before relink\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	forkval = fork();
	switch(forkval) {
	case -1: fprintf(stderr, "%s: fork fail\n", strerror(errno));
		exit(EXIT_FAILURE);
	case 0: 
	
		//if(!execv("/bin/ln", args)) {
		if(!(symlink("tempdir/dir1/secretfile", "tempdir/dir1/sl1"))) {
			fprintf(stderr, "should not have allowed execv ln\n");
			remove_directories();
			exit(EXIT_FAILURE);
		}
		else {
			exit(EXIT_SUCCESS);
		}
		break;
		
	default: 
		if(usleep(500000)) {
			fprintf(stderr, "%s: usleep failed\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		wait(&status);
		if(WEXITSTATUS(status)==EXIT_FAILURE) {
			remove_directories();
			return -1;
		}
		break;
	}
	remove_directories();
	return 0;


}


struct testcase testcase1a = {"1a", "For regular file, one process accesses and pins while another tries to modify by remove", test1a};
struct testcase testcase2a = {"2a", "For regular file, one process accesses and pins while another tries to modify the parent dir", test2a};
struct testcase testcase3a = {"3a", "symlink attack", test3a};


struct testcase **testcase;

void init_testcase() 
{
    int i = 0;

    testcase = malloc(sizeof(struct testcase) * 5);

    testcase[i++] = &testcase1a;
    testcase[i++] = &testcase2a;
    testcase[i++] = &testcase3a;
    testcase[i++] = NULL; 
}

void init_directories() {
	int fd1, fd2, fd3;
	if(mkdir("tempdir", FULL_PERM)) {
	}
	if(mkdir("tempdir/dir1", FULL_PERM)) {
	}
	if((fd1 = open("tempdir/dir1/f1", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM)) < 0) {
	}
	if(close(fd1)) {
	}
	if((fd2 = open("tempdir/dir1/f2", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM)) < 0) {
	}
	if(close(fd2)) {
       	}
	if((fd3 = open("tempdir/dir1/f3", O_WRONLY | O_CREAT | O_TRUNC, FULL_PERM)) < 0) {
	}
	if(close(fd3)) {
       	}
	if(symlink("f3", "tempdir/dir1/sl1")) {	}
	
}

void remove_directories() {
	if(remove("tempdir/dir1/sl1")) {
	}
	if(remove("tempdir/dir1/f1")) {
	}
	if(remove("tempdir/dir1/f2")) {
	}
	if(remove("tempdir/dir1/f3")) {
	}
	if(remove("tempdir/dir1/")) {
	}
	if(remove("tempdir/")) {
	}
}
