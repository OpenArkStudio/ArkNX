#include "TestEventsManager.h"

namespace ark
{

    void EventManagerTimeout(int thread_id, AFIThreadEvent* thread_event)
    {
        printf_s("[EventTimeout]thread_id=%d, event=%d.\n", thread_id, thread_event->GetEventID());
    }

    void UnitTestEventsManager(AFCThreadEventsManager& thread_event_manager)
    {
        thread_event_manager.Init(100, EventManagerTimeout);

        AFCThreadEvent* thread_event = new AFCThreadEvent();
        thread_event->Set(1000, "freeeyes", NULL, 1000);

        if (false == thread_event_manager.AddEvent(1, (AFIThreadEvent* )thread_event))
        {
            printf_s("[UnitTestEventsManager]AddEvent error, event=%d.\n", thread_event->GetEventID());
            delete thread_event;
            return;
        }

        AFIThreadEvent* thread_event_recv = NULL;

        if (true == thread_event_manager.GetEvent(1, (AFIThreadEvent* )thread_event_recv))
        {
            if (thread_event_recv->GetEventID() != 1000)
            {
                printf_s("[UnitTestEventsManager]GetEvent error, event=%d.\n", thread_event->GetEventID());
                delete thread_event_recv;
                return;
            }
        }
        else
        {
            printf_s("[UnitTestEventsManager]GetEvent error, event is NULL.\n");
            delete thread_event;
            return;
        }

        //test timout
        thread_event->Set(1001, "freeeyes", NULL, 1000);

        if (false == thread_event_manager.AddEvent(1, (AFIThreadEvent*)thread_event))
        {
            printf_s("[UnitTestEventsManager]AddEvent error, event=%d.\n", thread_event->GetEventID());
            return;
        }

        printf_s("[UnitTestEventsManager]Check OK.\n");
    }
}

