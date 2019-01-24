#include "AFCThread.h"


ark::AFCThread::AFCThread() : thread_id_(HANDEL_ERROR_VALUE), thread_mutex_(NULL), thread_cond_(NULL)
{
#if ARK_PLATFORM == PLATFORM_WIN
    thread_mutex_ = new CRITICAL_SECTION();
    thread_cond_ = new CONDITION_VARIABLE();
#else
    thread_mutex_ = new pthread_mutex_t();
    thread_cond_ = new pthread_cond_t();
#endif
}

ark::AFCThread::~AFCThread()
{
    if (NULL != thread_mutex_)
    {
        delete thread_mutex_;
        thread_mutex_ = NULL;
    }

    if (NULL != thread_cond_)
    {
        delete thread_cond_;
        thread_cond_ = NULL;
    }
}

bool ark::AFCThread::CreateThread(ThreadCallbackLogic thread_callback_logic, void* arg)
{
    thread_param_.thread_                = (AFIThread* )this;
    thread_param_.arg_                   = arg;
    thread_param_.thread_callback_logic_ = thread_callback_logic;

#if ARK_PLATFORM == PLATFORM_WIN
    unsigned int thread_id = 0;
    thread_id_ = (HANDLE)_beginthreadex(NULL, 0, ThreadCallbackRun, (PVOID)&thread_param_, 0, &thread_id);
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

bool ark::AFCThread::Pause()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SleepConditionVariableCS(reinterpret_cast<PCONDITION_VARIABLE>(thread_cond_),
                             reinterpret_cast<PCRITICAL_SECTION>(thread_mutex_),
                             INFINITE);
#else
    pthread_cond_wait(thread_cond_,
                      thread_mutex_);
#endif
    return true;
}

bool ark::AFCThread::Resume()
{
#if ARK_PLATFORM == PLATFORM_WIN
    LeaveCriticalSection(thread_mutex_);
#else
    pthread_mutex_unlock(thread_mutex_);
#endif

    return true;
}

void ark::AFCThread::Lock()
{
#if ARK_PLATFORM == PLATFORM_WIN
    EnterCriticalSection(thread_mutex_);
#else
    pthread_mutex_lock(thread_mutex_);
#endif
}

void ark::AFCThread::UnLock()
{
#ifdef WIN32
    LeaveCriticalSection(thread_mutex_);
#else
    pthread_mutex_unlock(thread_mutex_);
#endif
}

void ark::AFCThread::SaveLastRunTimeBegin()
{
    //save thread last run time begin
}

void ark::AFCThread::SaveLastRunTimeEnd()
{
    //save thread last run time End
}

ark::ThreadID ark::AFCThread::GetThreadID()
{
    return thread_id_;
}
