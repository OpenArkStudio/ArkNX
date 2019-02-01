#ifndef _TEST_EVENTSMANAGER_H
#define _TEST_EVENTSMANAGER_H

#include "../frame/app/AFCThreadEventsManager.h"

namespace ark
{
    inline void EventManagerTimeout(int thread_id, AFCThreadEvent thread_event);

    void UnitTestEventsManager(AFCThreadEventsManager& thread_event_manager);
}
#endif
