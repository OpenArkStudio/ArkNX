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

using namespace std;

namespace ark
{
    //Thread return
    enum ThreadReturn
    {
        ARK_THREAD_RETURN_ONCE = 0,
        ARK_THREAD_RETURN_CONTINUE,
        ARK_THREAD_RETURN_ERROR,
    };

    //thread logic function
    typedef ThreadReturn(*ThreadCallbackLogic)(void*);

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

    class AFIThread
    {
    public:
        AFIThread() {};
        virtual ~AFIThread() {}

        virtual void Lock() = 0;

        virtual void UnLock() = 0;

        virtual void SaveLastRunTimeBegin() = 0;

        virtual void SaveLastRunTimeEnd() = 0;
    };

    //thread param
    class AFCThreadParam
    {
    public:
        AFCThreadParam() : thread_(NULL), arg_(NULL) {};

    public:
        AFIThread* thread_;
        void*      arg_;
        ThreadCallbackLogic thread_callback_logic_;
    };

    //run thread logic
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned ThreadCallbackRun(void* arg)
#else
    void* ThreadCallbackRun(void* arg)
#endif
    {
        AFCThreadParam* thread_param = (AFCThreadParam*)arg;

        thread_param->thread_->Lock();

        while (true)
        {
            thread_param->thread_->SaveLastRunTimeBegin();
            ThreadReturn thread_return = thread_param->thread_callback_logic_(thread_param->arg_);
            thread_param->thread_->SaveLastRunTimeEnd();

            if (ARK_THREAD_RETURN_ONCE == thread_return)
            {
                //thread logic just run once
                break;
            }
            else if (ARK_THREAD_RETURN_ERROR == thread_return)
            {
                //write log
            }
        }

        thread_param->thread_->UnLock();

        return 0;
    };

    class AFCThread : public AFIThread
    {
    public:
        AFCThread();
        virtual ~AFCThread();

        bool CreateThread(ThreadCallbackLogic thread_callback_logic, void* arg);

        int KillThread();

        bool IsAlive();

        bool Pause();

        bool Resume();

        virtual void Lock();

        virtual void UnLock();

        virtual void SaveLastRunTimeBegin();

        virtual void SaveLastRunTimeEnd();

        ThreadID GetThreadID();

    private:
        ThreadID thread_id_;
        ThreadMutex* thread_mutex_;
        ThreadCond* thread_cond_;
        AFCThreadParam thread_param_;
        int64_t create_thread_time_;
        int64_t logic_begin_thread_time_;
        int64_t logic_end_thread_time_;
    };
}

#endif
