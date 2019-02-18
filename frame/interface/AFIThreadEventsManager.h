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

#ifndef _AFIEVENTTHREADMANAGER_H
#define _AFIEVENTTHREADMANAGER_H

#include "AFIThreadEvent.h"

namespace ark
{
    //main thread maintain interface
    class AFIThreadEventsManager
    {
    public:
        AFIThreadEventsManager() {};
        virtual ~AFIThreadEventsManager() {};

        virtual bool AddEvent(int thread_logic_id, AFIThreadEvent* thread_event) = 0;

        virtual AFIThreadEvent* GetEvent(int thread_logic_id) = 0;

        virtual bool GetEvents(int thread_logic_id, vector<AFIThreadEvent*>& thread_events_list) = 0;
    };

}

#endif //_AFITHREADMANAGER_H