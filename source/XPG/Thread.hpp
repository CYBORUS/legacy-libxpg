#ifndef XPGH_THREAD
#define XPGH_THREAD

#include "OperatingSystems.hpp"
#include "DataTypes.hpp"

#ifdef XPG_OS_UNIX
#include <pthread.h>
#endif

namespace XPG
{
    typedef volatile const bool& StopFlag;

    class Task
    {
        public:
            virtual void run(StopFlag) = 0;
    };

    class Thread
    {
        public:
            Thread();
            ~Thread();

            void start(Task* inTask = NULL);
            void launch();
            void wait();
            inline bool isRunning() { return mRunning; }
            inline void stop() { mStop = true; }

        private:
            Task* mTask;
            volatile bool mReady;
            volatile bool mRunning;
            volatile bool mStop;

#ifdef XPG_OS_WINDOWS
            DWORD mID;
            HANDLE mThread;

            static DWORD WINAPI createThread(LPVOID inData);
#elif defined(XPG_OS_UNIX)
            pthread_t mThread;
#endif
    };
}

#endif
