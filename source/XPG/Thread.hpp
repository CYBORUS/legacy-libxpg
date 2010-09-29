#ifndef XPGH_THREAD
#define XPGH_THREAD

#include "OperatingSystems.hpp"
#include "DataTypes.hpp"

#ifdef XPG_OS_UNIX
#include <pthread.h>
#endif

namespace XPG
{
    class Thread
    {
        public:
            Thread();
            virtual ~Thread();

            void runConcurrently();
            void wait();
            inline bool isRunning() { return mRunning; }
            inline void stop() { mStop = true; }
            inline bool stopRequested() { return mStop; }

            virtual void run() = 0;

        private:
            volatile bool mRunning;
            volatile bool mStop;

#ifdef XPG_OS_WINDOWS
            DWORD mID;
            HANDLE mThread;

            static DWORD WINAPI createThread(LPVOID inData);
#elif defined(XPG_OS_UNIX)
            pthread_t mThread;

            static void* createThread(void* inData);
#endif
    };
}

#endif
