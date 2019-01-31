#ifndef _AFITHREADEVENT_H
#define _AFITHREADEVENT_H

#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"

namespace ark
{
#define MAX_EVENT_PARAM_SIZE 200

    class AFIThreadEvent
    {
    public:
        AFIThreadEvent() {};

        virtual ~AFIThreadEvent() {};

        virtual void Set(int32_t event_id, const char* event_param, void* event_context, int32_t event_timeout = 0) = 0;

        virtual int32_t GetEventID() = 0;

        virtual void* GetContext() = 0;

        virtual char* GetParam() = 0;

        virtual bool IsTimeout(AFDateTime date_now) = 0;
    };
}

#endif
