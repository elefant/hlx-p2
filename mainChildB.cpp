#include "common.h"
#include "mainChildB.h"
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <string>
#include <math.h>

#define LOG(args...) log("mainChildB", ##args)

int mainChildB(SharedMemory* sm)  {
  LOG("Child process B starts.\n");

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

    std::vector<int> nums;
    int sum = 0;
    int squareSum = 0;
    for (int i = 0; i < sm->mCnt; i++) {
      int v = sm->mNums[i];
      nums.push_back(sm->mNums[i]);
      sum += v;
      squareSum += (v * v);
    }

    // Caculate the standard deviation.
    double mean = (double)sum / nums.size();
    double stdDev = sqrt((double)squareSum / nums.size() - mean * mean);

    std::sort(nums.begin(), nums.end());

    LOG("Sorted sequence: %s\n", vecToString(nums).c_str());
    LOG("Standard Deviation: %lf\n", stdDev);

    sm->mCnt = 0;
  }

  LOG("Child process B exits.\n");

  return 0;
}

