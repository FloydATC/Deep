
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


double unixtime_now()
{
  auto unixtime = std::chrono::system_clock::now().time_since_epoch();
  std::chrono::seconds seconds = std::chrono::duration_cast< std::chrono::seconds >(unixtime);
  std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(unixtime);
  return (double) seconds.count() + ((double)(ms.count() % 1000)/1000.0);
}

