#include "AFCThreadManager.h"

ark::AFCThreadManager::AFCThreadManager() : thread_timeout_(0), main_check_time_interval_(0)
{
#if ARK_PLATFORM == PLATFORM_WIN
    main_thread_mutex_ = new CRITICAL_SECTION();
#else
    main_thread_mutex_ = new pthread_mutex_t();
#endif
}

ark::AFCThreadManager::~AFCThreadManager()
{
    if (NULL != main_thread_mutex_)
    {
        delete main_thread_mutex_;
        main_thread_mutex_ = NULL;
    }
}

void ark::AFCThreadManager::Init(int main_check_time_interval, int64_t thread_timeout)
{
    main_check_time_interval_ = main_check_time_interval;
    thread_timeout_           = thread_timeout;

    //create main thread
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned int thread_id = 0;
    _beginthreadex(NULL, 0, MainThreadCallbackRun, (PVOID)(AFIThreadManager* )this, 0, &thread_id);
#else
    pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)(AFIThreadManager*)this);
#endif
}

bool ark::AFCThreadManager::CreateThread(int thread_logic_id, ThreadCallbackLogic thread_callback_logic, ThreadErrorLogic thread_callback_error, void* arg)
{
    Lock();
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        UnLock();
        return false;
    }

    AFCThread* thread_info = new AFCThread();

    if (NULL == thread_info)
    {
        UnLock();
        return false;
    }

    bool blret = thread_info->CreateThread(thread_logic_id, thread_callback_logic, thread_callback_error, arg);

    if (true == blret)
    {
        thread_list_.insert(mapThreadList::value_type(thread_logic_id, thread_info));
        UnLock();
        return true;
    }
    else
    {
        delete thread_info;
        UnLock();
        return false;
    }

}

bool ark::AFCThreadManager::KillThread(int thread_logic_id)
{
    Lock();
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        thread_info->KillThread();

        //recovery thread info
        thread_list_.erase(f);
        UnLock();
        return true;
    }
    else
    {
        UnLock();
        return false;
    }

}

bool ark::AFCThreadManager::IsAlive(int thread_logic_id)
{
    Lock();
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        UnLock();
        return thread_info->IsAlive();
    }
    else
    {
        UnLock();
        return false;
    }
}

bool ark::AFCThreadManager::Pause(int thread_logic_id)
{
    Lock();
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        UnLock();
        return thread_info->Pause();
    }
    else
    {
        UnLock();
        return false;
    }
}

bool ark::AFCThreadManager::Resume(int thread_logic_id)
{
    Lock();
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        UnLock();
        return thread_info->Resume();
    }
    else
    {
        UnLock();
        return false;
    }
}

void ark::AFCThreadManager::CheckThreadList()
{
    Lock();

    AFDateTime date_now;

    for (mapThreadList::iterator b = thread_list_.begin(); b != thread_list_.end(); ++b)
    {
        AFCThread* thread_info = (AFCThread*)b->second;

        if (ARK_THREAD_STATE_LOGIC_RUN_BEGIN == thread_info->GetThreadState())
        {
            int64_t date_interval = date_now - thread_info->GetLogicBeginThreadTime();

            if (date_interval >= thread_timeout_)
            {
                //thread timeout
                thread_info->ThreadTimeoutCallBack();
            }
        }
    }

    UnLock();
}

int ark::AFCThreadManager::GetMainThreadChekInterval()
{
    return main_check_time_interval_;
}

void ark::AFCThreadManager::Lock()
{
#if ARK_PLATFORM == PLATFORM_WIN
    EnterCriticalSection(main_thread_mutex_);
#else
    pthread_mutex_lock(main_thread_mutex_);
#endif
}

void ark::AFCThreadManager::UnLock()
{
#ifdef WIN32
    LeaveCriticalSection(main_thread_mutex_);
#else
    pthread_mutex_unlock(main_thread_mutex_);
#endif
}
