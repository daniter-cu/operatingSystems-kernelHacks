#include <stdlib.h>
#include <string.h>
#include "test.h"
 
struct testcase **testcase;
 
int main(int argc, char **argv) {
    char *sta, *end;
    int argi = 1, testi;
     
    init_testcase();
    init_directories(); /* OS HW6 */
    do {
	if (argi >= argc)
	    end = sta = NULL;
	else if (ISTEST(argv[argi]))
	    end = sta = argv[argi];
	else if (ISRANGE(argv[argi]))
	    end = (sta = argv[argi]) + 3;
	else {
	    fprintf(stderr, "invalid: %s\n", argv[argi]);
	    fprintf(stderr, "USAGE: ./test\n");
	    fprintf(stderr, " ./test [num] ...\n");
	    fprintf(stderr, " ./test [num]-[num] ...\n");
	    continue;
	}
	 
	for (testi = 0 ; testcase[testi] != NULL ; testi++) {
	    if (sta == NULL || end == NULL || (
			strncmp(sta, testcase[testi]->num, 2) <= 0 &&
			strncmp(end, testcase[testi]->num, 2) >= 0))
		 
		runtest(testcase[testi]);
	}
    } while (++argi < argc);
     
    return 0;
}


