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

#ifndef _AFCTHREADMANAGER_H
#define _AFCTHREADMANAGER_H

#include "base/AFMacros.hpp"
#include "AFCThread.h"
#include "interface/AFIMaintainThreadManager.h"
#include "interface/AFILogicThreadManager.h"

using namespace std;

#define CLOSE_THREAD_SLEEP_TIME 100

namespace ark
{
    class AFCLogicThreadManager : public AFIMaintainThreadManager, AFILogicThreadManager
    {
    public:
        typedef map<int, AFCThread*> mapThreadList;

        AFCLogicThreadManager();
        virtual ~AFCLogicThreadManager();

        void Close();

        void Init(int64_t main_check_time_interval, AFIPluginManager* plugin_manager);

        virtual bool CreateThread(int thread_logic_id,
                                  ThreadInit thread_init,
                                  ThreadCallbackLogic thread_callback_logic,
                                  ThreadErrorLogic thread_callback_error,
                                  ThreadExit thread_exit,
                                  void* arg);

        virtual bool KillThread(int thread_logic_id);

        virtual bool IsAlive(int thread_logic_id);

        virtual bool Pause(int thread_logic_id);

        virtual bool Resume(int thread_logic_id);

        virtual void CheckThreadList();

        virtual int64_t GetMainThreadCheckInterval();

    private:
        void Lock();

        void UnLock();

    private:
        mapThreadList     thread_list_;
        ThreadMutex*      main_thread_mutex_;
        int64_t           main_check_time_interval_;
        AFIPluginManager* plugin_manager_;
    };
}

#endif
