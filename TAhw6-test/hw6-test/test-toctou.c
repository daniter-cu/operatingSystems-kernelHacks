#define _REENTRANT
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "test.h"
#include "test-toctou.h"

long test1a(void) {

	init_file(1);
	if(access("temp.txt", F_OK)) {
		fprintf(stderr, "%s: access temp.txt failed\n", 
			strerror(errno));
		remove_file(1);
		return -1;
	}
	if(unlink("temp.txt")) {
		fprintf(stderr, "%s: unlink failed\n", strerror(errno));
		return -1;
	}
	else {
		fprintf(stderr, "unlink success\n");
		return 0;
	}
}
struct testcase testcase1a = {"1a", "can unlink file after access", test1a};

long test1b(void) {
	
	int status;

	init_file(1);
	if(access("temp.txt", F_OK)) {
		fprintf(stderr, "%s: access temp.txt failed\n", 
			strerror(errno));
		remove_file(1);
		return -1;
	}
	switch(fork()) {
		case -1:
			fprintf(stderr, "%s: fork failed\n", strerror(errno));
			remove_file(1);
			return -1;
		case 0:
			if(unlink("temp.txt")) {
				fprintf(stderr, "%s: unlink failed\n", 
					strerror(errno));
				exit(EXIT_SUCCESS);
			}
			fprintf(stderr, "unlink should fail...\n");
			exit(EXIT_FAILURE);
		default:
			wait(&status);
			remove_file(1);
			if(WEXITSTATUS(status) == EXIT_FAILURE)
				return -1;
			return 0;
	}
}
struct testcase testcase1b = {"1b", "can't unlink file after access", test1b};

long test1c(void) {

	int fd;

	init_file(2);
	if(access("link", F_OK)) {
		fprintf(stderr, "%s: access link failed\n",
			strerror(errno));
		remove_file(2);
		return -1;
	}
	if((fd = open("link", O_WRONLY | O_CREAT | O_TRUNC, 
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
		fprintf(stderr, "%s: open failed\n", strerror(errno));
		remove_file(2);
		return -1;
	}
	else {
		fprintf(stderr, "open success\n");
		close(fd);
		remove_file(2);
		return 0;
	}
}
struct testcase testcase1c = {"1c", "can open symbolic link after access", 
			      test1c};

long test1d(void) {

	int fd, status;

	init_file(2);
	if(access("link", F_OK)) {
		fprintf(stderr, "%s: access link failed\n",
			strerror(errno));
		remove_file(2);
		return -1;
	}
	switch(fork()) {
		case -1:
			fprintf(stderr, "%s: fork failed\n", strerror(errno));
			remove_file(2);
			return -1;
		case 0:
			if((fd = open("link", O_WRONLY | O_CREAT | O_TRUNC, 
				      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
				fprintf(stderr, "%s: open failed\n",
					strerror(errno));
				exit(EXIT_SUCCESS);
			}
			fprintf(stderr, "open should fail...\n");
			close(fd);
			exit(EXIT_FAILURE);
		default:
			wait(&status);
			remove_file(2);
			if(WEXITSTATUS(status) == EXIT_FAILURE)
				return -1;
			return 0;
	}
}
struct testcase testcase1d = {"1d", "can't open symbolic link after access",
			      test1d};

long test2a(void) {
	
	init_file(3);
	if(access("tempdir/temp.txt", F_OK)) {
		fprintf(stderr, "%s: access temp.txt failed\n",
			strerror(errno));
	}
	if(rmdir("tempdir")) {
		fprintf(stderr, "%s: unlink failed\n", strerror(errno));
		return -1;
	}
	else {
		fprintf(stderr, "unlink success\n");
		return 0;
	}
}
struct testcase testcase2a = {"2a", "try to access a nonexist file", test2a};

long test2b(void) {

	int status;

	init_file(4);
	if(access("tempdir/temp.txt", F_OK)) {
		fprintf(stderr, "%s: access temp.txt failed\n",
		strerror(errno));
		remove_file(4);
		return -1;
	}
	unlink("tempdir/temp.txt");
	switch(fork()) {
		case -1:
			fprintf(stderr, "%s: fork failed\n", strerror(errno));
			remove_file(4);
			return -1;
		case 0:
			if(rmdir("tempdir")) {
				fprintf(stderr, "%s: unlink failed\n",
					strerror(errno));
				exit(EXIT_SUCCESS);
			}
			fprintf(stderr, "unlink should fail...\n");
			exit(EXIT_FAILURE);
		default:
			wait(&status);
			remove_file(4);
			if(WEXITSTATUS(status) == EXIT_FAILURE)
				return -1;
			return 0;
	}
}
struct testcase testcase2b = {"2b", "try to unlink parent folder", test2b};

struct testcase **testcase;

#define TESTNUM 6

void init_testcase() {
	int i = 0;

	testcase = malloc(sizeof(struct testcase) * (TESTNUM + 1));

	testcase[i++] = &testcase1a;
	testcase[i++] = &testcase1b;
	testcase[i++] = &testcase1c;
	testcase[i++] = &testcase1d;
	testcase[i++] = &testcase2a;
	testcase[i++] = &testcase2b;
	testcase[i++] = NULL;
}

void init_file(int test) {

	int fd;

	switch(test) {
		case 1: 
			fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fd >= 0)
				close(fd);
			break;
		case 2:
			fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC,
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fd >= 0)
				close(fd);
			symlink("temp.txt", "link");
			break;
		case 3:
			mkdir("tempdir", S_IRUSR | S_IWUSR | S_IXUSR | 
			      S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | 
			      S_IWOTH | S_IXOTH);
			break;
		case 4:
			mkdir("tempdir", S_IRUSR | S_IWUSR | S_IXUSR |
			      S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH |
			      S_IWOTH | S_IXOTH);
			fd = open("tempdir/temp.txt", 
				  O_WRONLY | O_CREAT | O_TRUNC, 
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fd >= 0)
				close(fd);
	}
}

void remove_file(int test) {

	switch(test) {
		case 1:
			unlink("temp.txt");
			break;
		case 2:
			unlink("temp.txt");
			unlink("link");
			break;
		case 3:
			rmdir("tempdir");
			break;
		case 4:
			unlink("tempdir/temp.txt");
			rmdir("tempdir");
	}
}
