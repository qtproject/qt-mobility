#include "performance.h"

/************************** platform-specific parts **************************/

#ifdef WIN32

static double QueryPerformanceFrequency_wrapper()
{
    LARGE_INTEGER freq;

    Q_ASSERT(QueryPerformanceFrequency(&freq));

    return static_cast<double>(freq.QuadPart);
}

double ticksPerSecond = QueryPerformanceFrequency_wrapper();

perf_t perf_currentTime()
{
    LARGE_INTEGER ret;

    Q_ASSERT(QueryPerformanceCounter(&ret));

    return ret;
}

double perf_diffTime(perf_t start, perf_t end)
{
    return (end.QuadPart - start.QuadPart) / ticksPerSecond;
}

quint64 perf_diffTimeNative(perf_t start, perf_t end)
{
    return end.QuadPart - start.QuadPart;
}

#else

perf_t perf_currentTime()
{
    return QTime::currentTime();
}

double perf_diffTime(perf_t start, perf_t end)
{
    return start.msecsTo(end) / 1000.0;
}

quint64 perf_diffTimeNative(perf_t start, perf_t end)
{
    return start.msecsTo(end);
}

#endif

/************************ end platform-specific parts ************************/
