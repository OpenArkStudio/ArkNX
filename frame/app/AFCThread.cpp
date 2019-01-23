#include "AFCThread.h"


ark::AFCThread::AFCThread() : thread_id_(HANDEL_ERROR_VALUE)
{

}

ark::AFCThread::~AFCThread()
{

}

bool ark::AFCThread::CreateThread(ThreadCallbackRun thread_callback_run, void* arg)
{
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned int thread_id = 0;
    thread_id_ = (HANDLE)_beginthreadex(NULL, 0, thread_callback_run, (PVOID)arg, 0, &thread_id);
#else
    pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)&arg);
#endif

    if (thread_id_ != HANDEL_ERROR_VALUE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int ark::AFCThread::KillThread()
{
    if (HANDEL_ERROR_VALUE != thread_id_)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        CloseHandle(thread_id_);
        thread_id_ = HANDEL_ERROR_VALUE;
#else
        pthread_cancel(thread_id_);
#endif
    }

    return 0;
}

bool ark::AFCThread::IsAlive()
{
    if (HANDEL_ERROR_VALUE != thread_id_)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return true;
#else
        int kill_rc = pthread_kill(thread_id_, 0);

        if (kill_rc == ESRCH)
        {
            thread_id_ = HANDEL_ERROR_VALUE;
            return false;
        }
        else
        {
            return true;
        }

#endif
    }
    else
    {
        return false;
    }
}

ark::ThreadID ark::AFCThread::GetThreadID()
{
    return thread_id_;
}
