#include "AFCThreadManager.h"

ark::AFCThreadManager::AFCThreadManager() :
    main_check_time_interval_(0),
    plugin_manager_(NULL)
{
#if ARK_PLATFORM == PLATFORM_WIN
    main_thread_mutex_ = new CRITICAL_SECTION();
#else
    main_thread_mutex_ = new pthread_mutex_t();
#endif
}

ark::AFCThreadManager::~AFCThreadManager()
{
    Close();

    if (NULL != main_thread_mutex_)
    {
        delete main_thread_mutex_;
        main_thread_mutex_ = NULL;
    }
}

void ark::AFCThreadManager::Close()
{
    Lock();
    vector<AFCThread*> vec_thread_list;

    for (mapThreadList::iterator b = thread_list_.begin(); b != thread_list_.end(); ++b)
    {
        AFCThread* thread_info = (AFCThread*)b->second;
        thread_info->StopThread();
        vec_thread_list.push_back(thread_info);
    }

    if (vec_thread_list.size() > 0)
    {
        ARK_SLEEP(CLOSE_THREAD_SLEEP_TIME);
    }

    for (int i = 0; i < (int)vec_thread_list.size(); i++)
    {
        delete vec_thread_list[i];
    }

    vec_thread_list.clear();
    thread_list_.clear();
    UnLock();
}

void ark::AFCThreadManager::Init(int64_t main_check_time_interval, AFIPluginManager* plugin_manager)
{
    main_check_time_interval_ = main_check_time_interval;
    plugin_manager_           = plugin_manager;

    //create main thread
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned int thread_id = 0;
    _beginthreadex(NULL, 0, MainThreadCallbackRun, (PVOID)(AFIThreadManager* )this, 0, &thread_id);
#else
    pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)(AFIThreadManager*)this);
#endif
}

bool ark::AFCThreadManager::CreateThread(int thread_logic_id,
        ThreadInit thread_init,
        ThreadCallbackLogic thread_callback_logic,
        ThreadErrorLogic thread_callback_error,
        ThreadExit thread_exit,
        void* arg)
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

    bool blret = thread_info->CreateThread(thread_logic_id,
                                           thread_init,
                                           thread_callback_logic,
                                           thread_callback_error,
                                           thread_exit,
                                           arg,
                                           plugin_manager_);

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

            if (date_interval >= GetMainThreadChekInterval())
            {
                //thread timeout
                thread_info->ThreadTimeoutCallBack();
            }
        }
    }

    UnLock();
}

int64_t ark::AFCThreadManager::GetMainThreadChekInterval()
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
