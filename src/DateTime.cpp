
#include "DateTime.h"

#include <iostream>

Time time_now()
{
  return std::chrono::high_resolution_clock::now();
}


double time_elapsed_ms(Time current, Time then)
{
  double ms = std::chrono::duration_cast<std::chrono::milliseconds>(current - then).count();
  return ms;
}

