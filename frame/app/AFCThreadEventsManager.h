/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#ifndef _AFCTHREADEVENTSMANAGER_H
#define _AFCTHREADEVENTSMANAGER_H

#include "base/AFMacros.hpp"
#include "interface/AFIMaintainThreadManager.h"
#include "interface/AFIThread.h"
#include "interface/AFIThreadEventsManager.h"
#include "interface/AFIThreadWakeUp.h"
#include "AFCThreadEvent.h"
#include <map>
#include <deque>

using namespace std;

namespace ark
{
    typedef void(*EventTimeout)(int, AFIThreadEvent*);

    class AFCThreadEventsManager : public AFIMaintainThreadManager, public AFIThreadEventsManager
    {
    public:
        AFCThreadEventsManager();
        ~AFCThreadEventsManager();

        void Close();

        void Init(int max_thread_events_count, EventTimeout event_timeout_func, AFIThreadWakeUp* thread_wake_up);

        virtual void CheckThreadList();

        virtual int64_t GetMainThreadCheckInterval();

        virtual bool AddEvent(int thread_logic_id, AFIThreadEvent* thread_event);

        virtual AFIThreadEvent* GetEvent(int thread_logic_id);

        virtual bool GetEvents(int thread_logic_id, vector<AFIThreadEvent*>& thread_events_list);

    private:
        void Lock();

        void UnLock();

    private:
        typedef deque<AFIThreadEvent*> queEventList;
        typedef map<int, queEventList*> mapThreadEvents;

    private:
        int              max_thread_events_count_;
        int64_t          main_check_time_interval_;
        mapThreadEvents  map_thread_events_;
        ThreadMutex*     events_thread_mutex_;
        AFIThreadWakeUp* thread_wake_up_;
        EventTimeout     event_timeout_func_;
    };
}

#endif
