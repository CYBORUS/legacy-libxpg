#include "../Thread.hpp"

namespace XPG
{
    Thread::Thread() : mRunning(false), mStop(false)
    {
    }

    Thread::~Thread()
    {
        /// TODO: add more careful error handling
        CloseHandle(mThread);
    }

    void Thread::runConcurrently()
    {
        if (mRunning) return;
        mThread = CreateThread(NULL, 0, createThread, this, 0, &mID);
    }

    void Thread::wait()
    {
        WaitForSingleObject(mThread, INFINITE);
    }

    DWORD WINAPI Thread::createThread(LPVOID inData)
    {
        Thread* t = static_cast<Thread*>(inData);
        t->mStop = false;
        t->mRunning = true;
        t->run();
        t->mRunning = false;
        return 0;
    }
}
