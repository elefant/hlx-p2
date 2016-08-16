#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

int writeInt(int aFd, int aNum) {
  char buffer[100];
  sprintf(buffer, "%d\n", aNum);
  return write(aFd, buffer, strlen(buffer));
}

template<typename T> class AutoFree {
public:
  AutoFree(T* aBuffer) : mBuffer(aBuffer) {}
  ~AutoFree() { if (mBuffer) { free(mBuffer); } }
private:
  T* mBuffer;
};

bool readInt(FILE* aFp, int& aNum) {
  char *line = nullptr;
  size_t len = 0;
  ssize_t read = getline(&line, &len, aFp);

  AutoFree<char> freeOnExit(line);

  if (read < 0) {
    return false;
  }

  aNum = atoi(line);
  return true;;
}

std::vector<int> genRandomNumbers(int aCnt) {
  std::vector<int> nums;
  for (int i = 0; i < aCnt; i++) {
    nums.push_back(rand() % 50 + 1);
  }
  return nums;
}

int main(void)
{
  srand(time(nullptr));

  int fd[2];
  if (-1 == pipe(fd)) {
    return -1;
  }

  pid_t childpid;
  if (-1 == (childpid = fork())) {
    perror("fork");
    return -1;
  }

  if (0 == childpid) {
    // Child process closes up output side of pipe
    close(fd[1]);

    // Read in a string from the pipe
    FILE* fin = fdopen(fd[0], "r");
    while (1) {
      int cnt;
      if (!readInt(fin, cnt)) {
        break;
      }
      printf("Reading %d nums\n", cnt);

      int v;
      unsigned int sum = 0;
      for (int i = 0; i < cnt; i++) {
        readInt(fin, v);
        printf("Read %d\n", v);
        sum += v;
      }
      printf("Avg: %lf\n", (double)sum / cnt);
    }

    printf("Child exits.\n");
    return 0;
  } else {
    // Parent process closes up input side of pipe
    close(fd[0]);

    int fdout = fd[1];
    while (1) {
      int n;
      std::cin >> n;

      if (n == -1) {
        break;
      }
      if (n <= 0 || n > 50) {
        printf("Invalud input range: %d\n", n);
        continue;
      }

      writeInt(fdout, n);
      std::vector<int> nums = genRandomNumbers(n);
      for (auto v : nums) {
        writeInt(fdout, v);
      }
    }

    close(fdout);

    int status;
    int ret = waitpid(childpid, &status, 0);
    if (-1 == ret) {
      perror("waitpid error\n");
      return -1;
    }

    printf("Waited child returned.\n");
  }

  return 0;
}