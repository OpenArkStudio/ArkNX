#ifndef _TEST_EVENTSMANAGER_H
#define _TEST_EVENTSMANAGER_H

#include "../frame/app/AFCThreadEventsManager.h"

namespace ark
{
    void TestEventManagerTimeout(int thread_id, AFIThreadEvent* thread_event);

    void UnitTestEventsManager(AFCThreadEventsManager& thread_event_manager, AFIThreadWakeUp* thread_wake_up);
}
#endif
