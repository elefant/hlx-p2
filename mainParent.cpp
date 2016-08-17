#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <assert.h>

#include "common.h"
#include "mainChildA.h"
#include "mainChildB.h"

static int writeInt(int aFd, int aNum) {
  char buffer[100];
  sprintf(buffer, "%d\n", aNum);
  return write(aFd, buffer, strlen(buffer));
}

static std::vector<int> genRandomNumbers(int aCnt) {
  std::vector<int> nums;
  for (int i = 0; i < aCnt; i++) {
    nums.push_back(rand() % 50 + 1);
  }
  return nums;
}

int main(void)
{
  srand(time(nullptr));

  //--------------------------------------------------------
  // Fork childA and run its main routine.
  //--------------------------------------------------------

  // Create pipe to communicate with childA.
  int fd[2];
  if (-1 == pipe(fd)) {
    // Uable to create pipe for comminication.
    return -1;
  }

  pid_t pidChildA = fork();
  if (-1 == pidChildA) {
    // Unable to fork childA.
    return -1;
  }

  if (0 == pidChildA) {
    // We are ChildA! Close the output side.
    close(fd[1]);
    return mainChildA(fd[0]);
  }

  // Close the input side for the parent.
  close(fd[0]);

  //--------------------------------------------------------
  // Fork childB and run its main routine.
  //--------------------------------------------------------

  // Create shared memory to communucate with child B.
  const key_t SHM_KEY = 1234;
  int shmId = shmget(SHM_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
  SharedMemory* sm = (SharedMemory*) shmat(shmId, NULL, 0);
  if (-1 == sm) {
    // Unable to create shared memory for comminication.
    return -1;
  }
  printf("Parent sm address: %p\n", sm);

  // Reset sm->mCnt before forking childB.
  sm->mCnt = 0;
  pid_t pidChildB = fork();
  if (-1 == pidChildB) {
    // Unable to fork child B.
    return -1;
  }

  if (0 == pidChildB) {
    // We are ChildB. Obtain the shared memory and pass to childB.
    int shmId = shmget(SHM_KEY, sizeof(SharedMemory), 0666);
    SharedMemory* sm = (SharedMemory*) shmat(shmId, NULL, 0);
    return mainChildB(sm);
  }

  //----------------------------------------------------------
  // All settled down. Ready to go!
  //----------------------------------------------------------
  int fdout = fd[1];
  while (1) {
    // Wait until sm->mCnt is set to '0' by childB.
    if (sm->mCnt != 0) {
      usleep(100);
      continue;
    }

    // sm->mCnt is 0 now. Good to go!
    // Read a number from console.
    printf("Please input a integer between 1 and 50:\n");
    int n;
    std::cin >> n;

    // Check if asked to exit.
    if (n == -1) {
      break;
    }

    // Input value sanity check.
    if (n <= 0 || n > 50) {
      printf("Invalud input range: %d\n", n);
      continue;
    }

    // Set sm->mCnt to notify the child it's good to go!
    sm->mCnt = n;

    // Write out values to pipe and shared memory to be consumed.
    writeInt(fdout, n);
    std::vector<int> nums = genRandomNumbers(n);
    for (size_t i = 0; i < nums.size(); i++) {
      sm->mNums[i] = nums[i];
      writeInt(fdout, nums[i]);
    }
  }

  close(fdout); // Notify childA we are done.
  sm->mCnt = -1; // Notify childB we are done.

  int status;
  int ret = waitpid(pidChildA, &status, 0);
  if (-1 == ret) {
    perror("waitpid error\n");
    return -1;
  }

  int ret = waitpid(pidChildA, &status, 0);
  if (-1 == ret) {
    perror("waitpid error\n");
    return -1;
  }

  printf("Waited child returned.\n");

  return 0;
}