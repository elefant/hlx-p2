#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOG(args...) log("mainChildA", ##args)

namespace {

template<typename T> class AutoFree {
public:
  AutoFree(T* aBuffer) : mBuffer(aBuffer) {}
  ~AutoFree() { if (mBuffer) { free(mBuffer); } }
private:
  T* mBuffer;
};

} // end of unnamed namespace.

static bool readInt(FILE* aFp, int& aNum) {
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

int mainChildA(int aFdin) {
  LOG("Child process A starts.\n");

  FILE* fpin = fdopen(aFdin, "r");

  while (1) {
    int cnt;
    if (!readInt(fpin, cnt)) {
      break;
    }

    int v;
    unsigned int sum = 0;
    std::vector<int> nums;
    for (int i = 0; i < cnt; i++) {
      readInt(fpin, v);
      nums.push_back(v);
      sum += v;
    }

    LOG("Random Number Received: %s\n", vecToString(nums).c_str());
    LOG("Average: %lf\n", (double)sum / cnt);
  }

  LOG("Child process A exits.\n");

  return 0;
}