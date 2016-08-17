#include "common.h"
#include "mainChildB.h"
#include <stdio.h>

void mainChildB(SharedMemory* sm) {
  printf("Child sm address: %p\n", sm);

  while (1) {
    bool done = false;
    while (sm->mCnt <= 0) {
      if (sm->mCnt < 0) {
        // Parent asked me to exit.
        done = true;
        break;
      }
      // sm->mCnt is still '0'. Continue to wait.
      usleep(100);
    }

    if (done) {
      break;
    }

    printf("Child got mCnt %d\n", sm->mCnt);

    for (int i = 0; i < sm->mCnt; i++) {
      printf("%d, ", sm->mNums[i]);
    }
    printf("\n");

    sm->mCnt = 0;
  }
}