#include "performance.h"

/************************** time measurement **************************/

#ifdef Q_OS_WIN32

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

/************************** memory consumption **************************/

#ifdef Q_OS_WIN32

#include <psapi.h>

quint64 perf_currentMemUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;

    BOOL success = GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

    if (!success)
        return -1;

    return pmc.WorkingSetSize;
}

#else
#ifdef Q_OS_MAC

#include <malloc/malloc.h>

quint64 perf_currentMemUsage()
{
    malloc_statistics_t stats;

    malloc_zone_statistics(NULL, &stats);

    return stats.size_allocated; // maybe t.size_in_use?
}

#else
#if  _BSD_SOURCE || _SVID_SOURCE || (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && !(_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)


#include <unistd.h>

static char * sbrk_0_initial = (char*)sbrk(0);

quint64 perf_currentMemUsage()
{
    return (char*)sbrk(0)-sbrk_0_initial);
}

#else
quint64 perf_currentMemUsage()
{
    return 0;
}

#endif
#endif
#endif

/* TODO:
    mem
        - more platforms!
            - mac: malloc_zone_statistics
            -
    leaks?
        - boehm gc
*/
