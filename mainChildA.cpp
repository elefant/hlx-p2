#include "common.h"
#include <stdio.h>
#include <string.h>

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

void mainChildA(int aFdin) {
  FILE* fpin = fdopen(aFdin, "r");

  while (1) {
    int cnt;
    if (!readInt(fpin, cnt)) {
      break;
    }
    printf("Reading %d nums\n", cnt);

    int v;
    unsigned int sum = 0;
    for (int i = 0; i < cnt; i++) {
      readInt(fpin, v);
      printf("Read %d\n", v);
      sum += v;
    }
    printf("Avg: %lf\n", (double)sum / cnt);
  }
}