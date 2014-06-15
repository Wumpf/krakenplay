#include "time.h"

void Time::Initialize()
{
}

Time Time::Now()
{
  struct timespec sp;
  clock_gettime(CLOCK_MONOTONIC_RAW, &sp);

  return Time::Seconds( (double)sp.tv_sec + (double)(sp.tv_nsec / 1000000000.0));
}

