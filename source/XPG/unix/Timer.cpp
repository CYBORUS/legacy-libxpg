#include "../Timer.hpp"

#include <sys/time.h>
#ifdef XPGC_USE_NANOSLEEP
#   include <ctime>
#else
#   include <unistd.h>
#endif

namespace XPG
{
    static int64u mStart;

    void TimerStart()
    {
        timeval now;
        gettimeofday(&now, NULL);
        mStart = (now.tv_sec * 1000) + (now.tv_usec / 1000);
    }

    void Idle(int32u inMilliseconds)
    {
#ifdef XPGC_USE_NANOSLEEP
        struct timespec req = {0};
        time_t sec = (int)(inMilliseconds / 1000);
        inMilliseconds = inMilliseconds - (sec * 1000);
        req.tv_sec = sec;
        req.tv_nsec = inMilliseconds * 1000000L;
        while(nanosleep(&req, &req) == -1) continue;
#else
        usleep(inMilliseconds * 1000);
#endif
    }

    int64u GetTicks()
    {
        timeval now;
        gettimeofday(&now, NULL);
        int64u current = (now.tv_sec * 1000) + (now.tv_usec / 1000);
        return current - mStart;
    }
}
