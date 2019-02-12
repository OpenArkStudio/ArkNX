#include "AFCThreadEventsManager.h"

namespace ark
{
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned __stdcall EventsThreadCallbackRun(void* arg)
#else
    void* EventsThreadCallbackRun(void* arg)
#endif
    {
        AFIMaintainThreadManager* thread_manager = (AFIMaintainThreadManager* )arg;

        while (true)
        {
            thread_manager->CheckThreadList();
            ARK_SLEEP((int)thread_manager->GetMainThreadCheckInterval());
        }
    };

    AFCThreadEventsManager::AFCThreadEventsManager() : max_thread_events_count_(100),
        main_check_time_interval_(1000),
        events_thread_mutex_(NULL),
        thread_wake_up_(NULL)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        events_thread_mutex_ = new CRITICAL_SECTION();
        InitializeCriticalSection(events_thread_mutex_);
#else
        events_thread_mutex_ = new pthread_mutex_t();
        pthread_mutex_init(events_thread_mutex_, NULL);
#endif
    }

    AFCThreadEventsManager::~AFCThreadEventsManager()
    {
        Close();

        if (NULL != events_thread_mutex_)
        {
            delete events_thread_mutex_;
            events_thread_mutex_ = NULL;
        }
    }

    void AFCThreadEventsManager::Close()
    {
        Lock();

        for (mapThreadEvents::iterator b = map_thread_events_.begin(); b != map_thread_events_.end(); ++b)
        {
            queEventList* event_list = (queEventList*)b->second;
            event_list->clear();
            delete event_list;
        }

        map_thread_events_.clear();

        UnLock();
    }

    void AFCThreadEventsManager::Init(int max_thread_events_count, EventTimeout event_timeout_func, AFIThreadWakeUp* thread_wake_up)
    {
        max_thread_events_count_ = max_thread_events_count;
        event_timeout_func_      = event_timeout_func;
        thread_wake_up_          = thread_wake_up;

        //create maintain thread
#if ARK_PLATFORM == PLATFORM_WIN
        unsigned int thread_id = 0;
        _beginthreadex(NULL, 0, EventsThreadCallbackRun, (PVOID)(AFIMaintainThreadManager*)this, 0, &thread_id);
#else
        pthread_create(&thread_id_, NULL, EventsThreadCallbackRun, (void*)(AFIThreadManager*)this);
#endif
    }

    void AFCThreadEventsManager::CheckThreadList()
    {
        Lock();

        AFDateTime date_now;

        for (mapThreadEvents::iterator b = map_thread_events_.begin(); b != map_thread_events_.end(); ++b)
        {
            queEventList* event_list = (queEventList*)b->second;
            int thread_logic_id = (int)b->first;

            for (queEventList::iterator vec_b = event_list->begin(); vec_b != event_list->end();)
            {
                if ((*vec_b)->IsTimeout(date_now) == true)
                {
                    //need delete
                    event_timeout_func_(thread_logic_id, (*vec_b));
                    vec_b = event_list->erase(vec_b);
                }
                else
                {
                    vec_b++;
                }
            }
        }

        UnLock();
    }

    int64_t AFCThreadEventsManager::GetMainThreadCheckInterval()
    {
        return main_check_time_interval_;
    }

    bool AFCThreadEventsManager::AddEvent(int thread_logic_id, AFIThreadEvent* thread_event)
    {
        Lock();

        mapThreadEvents::iterator f = map_thread_events_.find(thread_logic_id);

        if (f == map_thread_events_.end())
        {
            //thread no exist
            queEventList* event_list = new queEventList();
            event_list->push_back(thread_event);
            map_thread_events_.insert(mapThreadEvents::value_type(thread_logic_id, event_list));
        }
        else
        {
            //thread exist
            queEventList* event_list = (queEventList*)f->second;

            if (event_list->size() >= max_thread_events_count_)
            {
                UnLock();
                return false;
            }
            else
            {
                event_list->push_back(thread_event);

                if (NULL != thread_wake_up_)
                {
                    thread_wake_up_->Wakeup(thread_logic_id);
                }
            }
        }

        UnLock();
        return true;
    }

    AFIThreadEvent* AFCThreadEventsManager::GetEvent(int thread_logic_id)
    {
        Lock();

        mapThreadEvents::iterator f = map_thread_events_.find(thread_logic_id);

        if (f == map_thread_events_.end())
        {
            UnLock();
            return NULL;
        }

        queEventList* event_list = (queEventList*)f->second;

        if (event_list->size() == 0)
        {
            UnLock();
            return NULL;
        }

        AFIThreadEvent* thread_event = event_list->front();
        event_list->pop_front();

        UnLock();
        return thread_event;
    }

    void AFCThreadEventsManager::Lock()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        EnterCriticalSection(events_thread_mutex_);
#else
        pthread_mutex_lock(events_thread_mutex_);
#endif
    }

    void AFCThreadEventsManager::UnLock()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        LeaveCriticalSection(events_thread_mutex_);
#else
        pthread_mutex_unlock(events_thread_mutex_);
#endif
    }

}
