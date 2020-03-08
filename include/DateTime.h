#ifndef DATETIME_H
#define DATETIME_H

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point Time;

Time time_now();
double time_elapsed_ms(Time current, Time then);
double unixtime_now();



#endif // DATETIME_H
