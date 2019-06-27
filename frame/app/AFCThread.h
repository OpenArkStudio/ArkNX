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

#ifndef _AFCTHREAD_H
#define _AFCTHREAD_H

#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"
#include "interface/AFIPluginContainer.h"
#include "interface/AFIThread.h"

namespace ark
{
    //thread param
    class AFCThreadParam
    {
    public:
        AFCThreadParam() : thread_(NULL) {};

    public:
        AFIThread*          thread_;
        AFIPluginContainer* plugin_container_;
    };

    class AFCThread : public AFIThread
    {
    public:
        AFCThread();
        virtual ~AFCThread();

        bool CreateThread(AFIPluginContainer* plugin_container,
                          AFIThreadEventsManager* event_manager,
                          AFILogicThreadManager* logic_manager);

        int KillThread();

        void StopThread();

        bool IsAlive();

        bool Pause();

        bool Resume();

        virtual int GetThreadLogicID();

        virtual void Lock();

        virtual void UnLock();

        virtual void SaveLastRunTimeBegin();

        virtual void SaveLastRunTimeEnd();

        virtual void SetThreadState(ThreadState thread_state);

        virtual ThreadState GetThreadState();

        virtual AFIManager* GetManager();

        virtual void SetCond(int interval_timeout = 0);

        virtual ThreadEventGetType GetThreadEventGetType();

        AFDateTime GetCreatehreadTime();

        AFDateTime GetLogicBeginThreadTime();

        AFDateTime GetLogicEndThreadTime();

        ThreadID GetThreadID();

        void ThreadTimeoutCallBack();

    private:
        int                thread_logic_id_;
        ThreadID           thread_id_;
        ThreadMutex*       thread_mutex_;
        ThreadCond*        thread_cond_;
        AFCThreadParam     thread_param_;
        AFDateTime         create_thread_time_;
        AFDateTime         logic_begin_thread_time_;
        AFDateTime         logic_end_thread_time_;
        ThreadState        thread_state_;
        AFIManager         manager_;
        ThreadEventGetType thread_event_get_type_;
    };
}

#endif
