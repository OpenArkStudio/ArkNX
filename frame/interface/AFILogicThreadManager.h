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

#ifndef _AFILOGICTHREADMANAGER_H
#define _AFILOGICTHREADMANAGER_H

#include "interface/AFIThread.h"

namespace ark
{
    //plug manager use interface
    class AFILogicThreadManager
    {
    public:
        AFILogicThreadManager() {};
        virtual ~AFILogicThreadManager() {};

        virtual bool CreateThread(int thread_logic_id,
                                  ThreadInit thread_init,
                                  ThreadCallbackLogic thread_callback_logic,
                                  ThreadErrorLogic thread_callback_error,
                                  ThreadExit thread_exit,
                                  void* arg) = 0;

        virtual bool KillThread(int thread_logic_id) = 0;

        virtual bool IsAlive(int thread_logic_id) = 0;

        virtual bool Pause(int thread_logic_id) = 0;

        virtual bool Resume(int thread_logic_id) = 0;
    };

}

#endif //_AFITHREADMANAGER_H