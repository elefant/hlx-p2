#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include <vector>
#include <string>

struct SharedMemory {
  int mCnt;
  int mNums[50];
};

void log(const char* aModule,
         const char* aFormat,
         ...);

std::string vecToString(const std::vector<int>& aVector);

#endif