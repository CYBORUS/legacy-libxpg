#ifndef XPGH_MUTEX
#define XPGH_MUTEX

#include "OperatingSystems.hpp"

#ifdef XPG_OS_UNIX
#include <pthread.h>
#endif

namespace XPG
{
    class Mutex
    {
        public:
            Mutex();
            ~Mutex();

            void lock();
            void unlock();

        private:
#ifdef XPG_OS_WINDOWS
            HANDLE mMutex;
#elif defined(XPG_OS_UNIX)
            pthread_mutex_t mMutex;
#endif
    };
}

#endif

