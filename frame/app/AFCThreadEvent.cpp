#include "AFCThreadEvent.h"

namespace ark
{

    AFCThreadEvent::AFCThreadEvent(int from_thread_id, int to_thread_id) : event_id_(0),
        event_priority_(ARK_EVENT_PRIORITY_COSTOM),
        event_context_(NULL),
        event_timeout_(0)
    {
        event_param_[0] = '\0';
        from_thread_id_ = from_thread_id;
        to_thread_id_   = to_thread_id;
    }

    AFCThreadEvent::~AFCThreadEvent()
    {

    }

    void AFCThreadEvent::Set(int32_t event_id, const char* event_param, void* event_context, int32_t event_timeout, EventPriority event_priority)
    {
        event_id_ = event_id;
#if ARK_PLATFORM == PLATFORM_WIN
        sprintf_s(event_param_, MAX_EVENT_PARAM_SIZE, "%s", event_param);
#else
        sprintf(event_param_, "%s", event_param);
#endif
        event_context_ = event_context;

        event_timeout_ = event_timeout;

        event_priority_ = event_priority;

        event_insert_.update();
    }

    int32_t AFCThreadEvent::GetEventID()
    {
        return event_id_;
    }

    void* AFCThreadEvent::GetContext()
    {
        return event_context_;
    }

    char* AFCThreadEvent::GetParam()
    {
        return event_param_;
    }

    bool AFCThreadEvent::IsTimeout(AFDateTime date_now)
    {
        if (0 == event_timeout_)
        {
            return false;
        }

        int64_t date_interval = date_now - event_insert_;

        if (date_interval >= event_timeout_)
        {
            //event timeout
            return true;
        }
        else
        {
            return false;
        }
    }

    EventPriority AFCThreadEvent::GetPriority()
    {
        return event_priority_;
    }

    int AFCThreadEvent::GetFromThreadID()
    {
        return from_thread_id_;
    }

    int AFCThreadEvent::GetToThreadID()
    {
        return to_thread_id_;
    }

}
