#ifndef _AFCTHREADEVENT_H
#define _AFCTHREADEVENT_H

#include "interface/AFIThreadEvent.h"

namespace ark
{
#define MAX_EVENT_PARAM_SIZE 200

    class AFCThreadEvent : public AFIThreadEvent
    {
    public:
        AFCThreadEvent(int from_thread_id, int to_thread_id);

        virtual ~AFCThreadEvent();

        virtual void Set(int32_t event_id,
                         const char* event_param,
                         void* event_context,
                         int32_t event_timeout = 0,
                         EventPriority event_priority = ARK_EVENT_PRIORITY_COSTOM);

        virtual int32_t GetEventID();

        virtual void* GetContext();

        virtual char* GetParam();

        virtual bool IsTimeout(AFDateTime date_now);

        virtual EventPriority GetPriority();

        virtual int GetFromThreadID();

        virtual int GetToThreadID();

    private:
        int32_t       event_id_;
        EventPriority event_priority_;
        void*         event_context_;
        int32_t       event_timeout_;
        char          event_param_[MAX_EVENT_PARAM_SIZE];
        AFDateTime    event_insert_;
        int           from_thread_id_;
        int           to_thread_id_;

    };
}

#endif
