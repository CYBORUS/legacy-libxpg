#include "../Thread.hpp"

namespace XPG
{
    Thread::Thread() : mRunning(false), mStop(false)
    {
    }

    Thread::~Thread()
    {
    }

    void Thread::runConcurrently()
    {
        if (mRunning) return;
        pthread_create(&mThread, NULL, createThread, this);
    }

    void Thread::wait()
    {
        pthread_join(mThread, NULL);
    }

    void* Thread::createThread(void* inData)
    {
        Thread* t = static_cast<Thread*>(inData);
        t->mStop = false;
        t->mRunning = true;
        t->run();
        t->mRunning = false;
        return 0;
    }
}
