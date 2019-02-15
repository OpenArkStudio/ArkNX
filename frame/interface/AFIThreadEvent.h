#ifndef _AFITHREADEVENT_H
#define _AFITHREADEVENT_H

#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"

namespace ark
{
#define MAX_EVENT_PARAM_SIZE 200

    enum EventPriority
    {
        ARK_EVENT_PRIORITY_COSTOM = 0,
        ARK_EVENT_PRIORITY_SYNCHRONIZE,
    };

    class AFIThreadEvent
    {
    public:
        AFIThreadEvent() {};

        virtual ~AFIThreadEvent() {};

        virtual void Set(int32_t event_id,
                         const char* event_param,
                         void* event_context,
                         int32_t event_timeout = 0,
                         EventPriority event_priority = ARK_EVENT_PRIORITY_COSTOM) = 0;

        virtual int32_t GetEventID() = 0;

        virtual void* GetContext() = 0;

        virtual char* GetParam() = 0;

        virtual bool IsTimeout(AFDateTime date_now) = 0;

        virtual int GetFromThreadID() = 0;

        virtual int GetToThreadID() = 0;

        virtual EventPriority GetPriority() = 0;
    };
}

#endif
