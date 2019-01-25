#include "AFCThread.h"


ark::AFCThread::AFCThread() :
    thread_logic_id_(0),
    thread_id_(HANDEL_ERROR_VALUE),
    thread_mutex_(NULL),
    thread_cond_(NULL),
    thread_state_(ARK_THREAD_STATE_NONE)
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

bool ark::AFCThread::CreateThread(int thread_logic_id, ThreadCallbackLogic thread_callback_logic, ThreadErrorLogic thread_callback_error, void* arg)
{
    if (ARK_THREAD_STATE_NONE != thread_state_)
    {
        return false;
    }

    thread_param_.thread_                = (AFIThread*)this;
    thread_param_.arg_                   = arg;
    thread_param_.thread_callback_logic_ = thread_callback_logic;
    thread_param_.thread_error_logic_    = thread_callback_error;

    thread_state_                        = ARK_THREAD_STATE_INIT;
    thread_logic_id_                     = thread_logic_id;
    thread_error_logic_                  = thread_callback_error;

#if ARK_PLATFORM == PLATFORM_WIN
    unsigned int thread_id = 0;
    thread_id_ = (HANDLE)_beginthreadex(NULL, 0, ThreadCallbackRun, (PVOID)&thread_param_, 0, &thread_id);
#else
    pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)&arg);
#endif

    if (thread_id_ != HANDEL_ERROR_VALUE)
    {
        create_thread_time_.update();
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
    logic_begin_thread_time_.update();
}

void ark::AFCThread::SaveLastRunTimeEnd()
{
    //save thread last run time End
    logic_end_thread_time_.update();
}

ark::AFDateTime ark::AFCThread::GetCreatehreadTime()
{
    return create_thread_time_;
}

ark::AFDateTime ark::AFCThread::GetLogicBeginThreadTime()
{
    return logic_begin_thread_time_;
}

ark::AFDateTime ark::AFCThread::GetLogicEndThreadTime()
{
    return logic_end_thread_time_;
}

ark::ThreadID ark::AFCThread::GetThreadID()
{
    return thread_id_;
}

void ark::AFCThread::ThreadTimeoutCallBack()
{
    thread_error_logic_(thread_logic_id_,
                        ARK_THREAD_LOGIC_TIMEOUT,
                        0,
                        NULL);
}

ark::ThreadState ark::AFCThread::GetThreadState()
{
    return thread_state_;
}

int ark::AFCThread::GetThreadLogicID()
{
    return thread_logic_id_;
}

void ark::AFCThread::SetThreadState(ThreadState thread_state)
{
    thread_state_ = thread_state;
}
