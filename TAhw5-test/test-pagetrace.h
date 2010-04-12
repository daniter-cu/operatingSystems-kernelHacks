#ifndef __TEST_ATOMIC_H
#define __TEST_ATOMIC_H

#include <linux/unistd.h>

_syscall0(pid_t, gettid); 
_syscall2(int, start_trace, unsigned long, start, size_t, size);
_syscall0(int, stop_trace);
_syscall2(int, get_trace, pid_t, tid, int *, wcount);

#endif

