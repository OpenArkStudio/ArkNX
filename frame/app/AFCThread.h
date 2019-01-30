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
#include "interface/AFIPluginManager.h"
#include "interface/AFIThread.h"

#if ARK_PLATFORM == PLATFORM_WIN
#include <windows.h>
#include <process.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#endif

namespace ark
{

#if ARK_PLATFORM == PLATFORM_WIN
    typedef HANDLE ThreadID;
    typedef CRITICAL_SECTION ThreadMutex;
    typedef CONDITION_VARIABLE ThreadCond;
#define HANDEL_ERROR_VALUE INVALID_HANDLE_VALUE
#else
    typedef pthread_t ThreadID;
    typedef pthread_mutex_t ThreadMutex;
    typedef pthread_cond_t ThreadCond;
#define HANDEL_ERROR_VALUE -1
#endif

    typedef void(*ThreadInit)(int, AFIPluginManager* plugin_manager);

    typedef ThreadReturn(*ThreadCallbackLogic)(int&, void*);

    typedef ThreadError(*ThreadErrorLogic)(int, ThreadLogicErrorType, int, void*);

    typedef void(*ThreadExit)(int, AFIPluginManager* plugin_manager);


    //thread param
    class AFCThreadParam
    {
    public:
        AFCThreadParam() : thread_(NULL), arg_(NULL) {};

    public:
        AFIThread* thread_;
        void*      arg_;
        ThreadCallbackLogic thread_callback_logic_;
        ThreadErrorLogic    thread_error_logic_;
        ThreadExit          thread_exit_;
        ThreadInit          thread_init_;
    };

    class AFCThread : public AFIThread
    {
    public:
        AFCThread();
        virtual ~AFCThread();

        bool CreateThread(int thread_logic_id,
                          ThreadInit thread_init,
                          ThreadCallbackLogic thread_callback_logic,
                          ThreadErrorLogic thread_callback_error,
                          ThreadExit thread_exit,
                          void* arg,
                          AFIPluginManager* plugin_manager);

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

        virtual AFIPluginManager* GetPluginManager();

        AFDateTime GetCreatehreadTime();

        AFDateTime GetLogicBeginThreadTime();

        AFDateTime GetLogicEndThreadTime();

        ThreadID GetThreadID();

        void ThreadTimeoutCallBack();

    private:
        int      thread_logic_id_;
        ThreadID thread_id_;
        ThreadMutex* thread_mutex_;
        ThreadCond* thread_cond_;
        AFCThreadParam thread_param_;
        AFDateTime create_thread_time_;
        AFDateTime logic_begin_thread_time_;
        AFDateTime logic_end_thread_time_;
        ThreadState thread_state_;
        ThreadErrorLogic thread_error_logic_;
        AFIPluginManager* plugin_manager_;
    };
}

#endif
