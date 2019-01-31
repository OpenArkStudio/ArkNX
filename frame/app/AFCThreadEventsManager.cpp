#include "AFCThreadEventsManager.h"

namespace ark
{
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned __stdcall EventsThreadCallbackRun(void* arg)
#else
    void* EventsThreadCallbackRun(void* arg)
#endif
    {
        AFIMaintainThreadManager* thread_manager = (AFIMaintainThreadManager*)arg;

        while (true)
        {
            thread_manager->CheckThreadList();
            ARK_SLEEP((int)thread_manager->GetMainThreadCheckInterval());
        }
    };

    AFCThreadEventsManager::AFCThreadEventsManager() : max_thread_events_count_(100),
        max_event_timeout_(1000),
        main_check_time_interval_(1000),
        events_thread_mutex_(NULL)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        events_thread_mutex_ = new CRITICAL_SECTION();
#else
        events_thread_mutex_ = new pthread_mutex_t();
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
            vecEventList* event_list = (vecEventList*)b->second;
            event_list->clear();
            delete event_list;
        }

        map_thread_events_.clear();

        UnLock();
    }

    void AFCThreadEventsManager::Init(int max_thread_events_count, int max_event_timeout)
    {
        max_thread_events_count_ = max_thread_events_count;
        max_event_timeout_       = max_event_timeout;

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
            vecEventList* event_list = (vecEventList*)b->second;

            for (vecEventList::iterator vec_b = event_list->begin(); vec_b != event_list->end();)
            {
                if ((*vec_b).IsTimeout(date_now) == true)
                {
                    //need delete
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
