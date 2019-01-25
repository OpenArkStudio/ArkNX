#include "AFCThreadManager.h"

ark::AFCThreadManager::AFCThreadManager()
{

}

ark::AFCThreadManager::~AFCThreadManager()
{

}

bool ark::AFCThreadManager::CreateThread(int thread_logic_id, ThreadCallbackLogic thread_callback_logic, ThreadErrorLogic thread_callback_error, void* arg)
{
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        return false;
    }

    AFCThread* thread_info = new AFCThread();

    if (NULL == thread_info)
    {
        return false;
    }

    bool blret = thread_info->CreateThread(thread_logic_id, thread_callback_logic, thread_callback_error, arg);

    if (true == blret)
    {
        thread_list_.insert(mapThreadList::value_type(thread_logic_id, thread_info));
        return true;
    }
    else
    {
        delete thread_info;
        return false;
    }

}

bool ark::AFCThreadManager::KillThread(int thread_logic_id)
{
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        thread_info->KillThread();

        //recovery thread info
        thread_list_.erase(f);
        return true;
    }
    else
    {
        return false;
    }

}

bool ark::AFCThreadManager::IsAlive(int thread_logic_id)
{
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        return thread_info->IsAlive();
    }
    else
    {
        return false;
    }
}

bool ark::AFCThreadManager::Pause(int thread_logic_id)
{
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        return thread_info->Pause();
    }
    else
    {
        return false;
    }
}

bool ark::AFCThreadManager::Resume(int thread_logic_id)
{
    mapThreadList::iterator f = thread_list_.find(thread_logic_id);

    //find esist thread
    if (thread_list_.end() != f)
    {
        AFCThread* thread_info = (AFCThread*)f->second;
        return thread_info->Resume();
    }
    else
    {
        return false;
    }
}
