#ifndef _TEST_H_
#define _TEST_H_

#include <stdio.h>
#include <string.h>

struct testcase {
	char *num;
	char *des;
	long (*test)(void);
};

extern struct testcase **testcase;
extern void init_testcase(void);
extern struct testcase *current_testcase;

extern int ISGOODTESTNUM(char *);

#define ISTEST(id) (strlen(id) == 2 && ISGOODTESTNUM(id))
#define ISRANGE(id) \
	(strlen(id) == 5 && ISGOODTESTNUM(id) && id[2] == '-' && \
	 ISGOODTESTNUM(id + 3))

extern void runtest(struct testcase *);

#define PRINT(...) { \
	fprintf(stderr, "[%s] ", current_testcase->num); \
	fprintf(stderr, __VA_ARGS__); }

#endif /* _TEST_H_ */
