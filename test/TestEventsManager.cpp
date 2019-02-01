#include "TestEventsManager.h"

namespace ark
{

    void EventManagerTimeout(int thread_id, AFCThreadEvent thread_event)
    {
        printf_s("[EventTimeout]thread_id=%d, event=%d.\n", thread_id, thread_event.GetEventID());
    }

    void UnitTestEventsManager(AFCThreadEventsManager& thread_event_manager)
    {
        thread_event_manager.Init(100, EventManagerTimeout);

        AFCThreadEvent thread_event;
        thread_event.Set(1000, "freeeyes", NULL, 1000);

        if (false == thread_event_manager.AddEvent(1, thread_event))
        {
            printf_s("[UnitTestEventsManager]AddEvent error, event=%d.\n", thread_event.GetEventID());
            return;
        }

        AFCThreadEvent thread_event_recv;

        if (true == thread_event_manager.GetEvent(1, thread_event_recv))
        {
            if (thread_event_recv.GetEventID() != thread_event.GetEventID())
            {
                printf_s("[UnitTestEventsManager]GetEvent error, event=%d.\n", thread_event.GetEventID());
                return;
            }
        }
        else
        {
            printf_s("[UnitTestEventsManager]GetEvent error, event is NULL.\n");
            return;
        }

        //test timout
        thread_event.Set(1001, "freeeyes", NULL, 1000);

        if (false == thread_event_manager.AddEvent(1, thread_event))
        {
            printf_s("[UnitTestEventsManager]AddEvent error, event=%d.\n", thread_event.GetEventID());
            return;
        }

        printf_s("[UnitTestEventsManager]Check OK.\n");
    }
}

