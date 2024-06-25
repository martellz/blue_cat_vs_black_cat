#ifdef LIBEIGEN3_DEV
// in ubuntu, just install eigen3 by: sudo apt install libeigen3-dev
#include<eigen3/Eigen/Core>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;
#else
#include <Eigen/Core>
#endif


#ifndef INPUTNUMBER
#define INPUTNUMBER
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

int benchmark();

struct Timer {
  std::chrono::system_clock::time_point start, end;
  std::chrono::duration<float> duration;

  Timer() {
    start = std::chrono::system_clock::now();
  }

  ~Timer() {
    end = std::chrono::system_clock::now();
    duration = end - start;
    std::cout << "Timer took " << duration.count() << "s" << std::endl;
  }
};

// TODO: implement the solution to this class
class Solution {};

#endif // INPUTNUMBER