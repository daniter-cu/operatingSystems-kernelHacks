#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <asm-i386/unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sched.h>
_syscall3(long, setprob, int, color1, int, color2, int, prob);
_syscall2(long, getprob, int, color1, int, color2);
_syscall2(long, setcolor, int, pid, int, color);
_syscall1(long, getcolor, int, pid);
_syscall0(long, gettid);

int main(int argc, char **argv) {
  int i;
  printf("color = 3, prob = 5 between 3 and 0\n");
  if(setcolor(gettid(), 3)) printf("holy crap\n");
  if(setprob(3,0,5)) printf("more crap\n");
  struct sched_param param;
  param.sched_priority = 0;
  
  if(sched_setscheduler(0,4, &param) < 0) printf("even more crap\n");
  
  sched_yield();

  for(i = 0; i < 10; ++i) {
    printf("%s %d\n", "hello", i);
  }

  

  
  /* pthread_t t1, t2, t3; */

  /* printf("%s\n", "testing setprob"); */
  /* if(setprob(-1,0,5)) { */
  /*   printf("%s %s\n", "invalid color1 lower bound: ", strerror(errno)); */
  /* } */
  /* if(setprob(5,0,5)) { */
  /*   printf("%s %s\n", "invalid color1 upper bound: ", strerror(errno)); */
  /* } */
  /* if(setprob(0,-1,5)) { */
  /*   printf("%s %s\n", "invalid color2 lower bound: ", strerror(errno)); */
  /* } */
  /* if(setprob(0,5,5)) { */
  /*   printf("%s %s\n", "invalid color2 upper bound: ", strerror(errno)); */
  /* } */
  /* if(setprob(0,0,11)) { */
  /*   printf("%s %s\n", "invalid prob upper bound: ", strerror(errno)); */
  /* } */
  /* if(setprob(0,0,-1)) { */
  /*   printf("%s %s\n", "invalid prob lower bound: ", strerror(errno)); */
  /* } */
  /* printf("\n"); */
  
  /* printf("%s\n", "testing getprob after bad values"); */
  /* printf("%s %ld\n", "getprob(0,0))", getprob(0,0)); */

  /* printf("\n"); */

  /* printf("%s\n", "setting the following color probabilities"); */
  /* printf("%s\n", "setprob(0,1,1))"); */
  /* if(setprob(0,1,1)) { */
  /*   printf("%s\n", strerror(errno)); */
  /* } */
  /* else { */
  /*   printf("success!\n"); */
  /* } */
  /* printf("%s\n", "setprob(0,0,3)"); */
  /* if(setprob(0,0,3)) { */
  /*   printf("%s\n", strerror(errno)); */
  /* } */
  /* else { */
  /*   printf("success!\n"); */
  /* } */
  /* printf("\n"); */

  /* printf("%s\n", "testing getprob.  check the returns!"); */
  /* printf("%s: %ld\n", "getprob(0,0)", getprob(0,0)); */
  /* printf("%s: %ld\n", "getprob(0,1)", getprob(0,1)); */

  /* printf("\n"); */
  /* printf("%s\n", "testing setcolor with invalid pid"); */
  /* if(setcolor(-1, 4)) { */
  /*   printf("%s %s\n", "invalid pid", strerror(errno)); */
  /* } */
  /* printf("%s\n", "testing setcolor(gettid(), 4) with current pid"); */
  /* if(setcolor(gettid(), 4)) { */
  /*   printf("%s\n", "this should not have failed"); */
  /* } */
  /* else { */
  /*   printf("success!\n"); */
  /* } */

  /* printf("%s\n", "testing getcolor(gettid())"); */
  /* if(getcolor(gettid()) != 4) { */
  /*   printf("%s: %s\n", "problem", strerror(errno)); */
  /* } */
  /* else { */
  /*   printf("success!\n"); */
  /* } */
    
  return 0;
}
