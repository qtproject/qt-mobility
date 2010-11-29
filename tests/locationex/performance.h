#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <QtGlobal>

#ifdef WIN32
#include <windows.h>
typedef LARGE_INTEGER perf_t;
#else
#include <QTime>
typedef QTime perf_t;
#endif

perf_t perf_currentTime();
double perf_diffTime(perf_t start, perf_t end);
quint64 perf_diffTimeNative(perf_t start, perf_t end);

#endif // PERFORMANCE_H
