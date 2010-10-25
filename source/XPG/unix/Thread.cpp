#include "../Thread.hpp"
#include <cerrno>

namespace XPG
{
    void* createThread(void* inData)
    {
        Thread* t = static_cast<Thread*>(inData);
        t->launch();
        pthread_exit(NULL);
        return NULL;
    }

    Thread::Thread() : mTask(NULL), mReady(false), mRunning(false), mStop(false)
    {
    }

    Thread::~Thread()
    {
    }

    void Thread::start(Task* inTask)
    {
        if (mRunning) return;

        if (inTask)
            mTask = inTask;
        else if (!mTask)
            return;

        mReady = true;
        mRunning = true;
        switch (pthread_create(&mThread, NULL, createThread, this))
        {
            case 0: break; // no error

            case EAGAIN: // The system lacked resources
            case EINVAL: // invalid attributes
            case EPERM: // Caller does not have appropriate permission
            default: // something else went wrong
            {
                mRunning = false;
            }
        }
    }

    void Thread::launch()
    {
        // This function may only ever be called from createThread (which is
        // called from Thread::start).
        if (!mReady) return;
        mReady = false;
        mStop = false;
        mTask->run(mStop);
        mRunning = false;
    }

    void Thread::wait()
    {
        switch (pthread_join(mThread, NULL))
        {
            case 0: break; // no error

            case EINVAL: break; // mThread does not refer to a joinable thread
            case ESRCH: break; // No thread found corresponding to mThread
            case EDEADLK: break; // A deadlock was detected.
            default: {} // something else went wrong
        }
    }
}
