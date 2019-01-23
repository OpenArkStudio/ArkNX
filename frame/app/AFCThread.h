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
#if ARK_PLATFORM == PLATFORM_WIN
    typedef unsigned (*ThreadCallbackRun)(LPVOID);
    typedef HANDLE ThreadID;
#define HANDEL_ERROR_VALUE INVALID_HANDLE_VALUE
#else
    typedef void* (*ThreadCallbackRun)(void*);
    typedef pthread_t ThreadID;
#define HANDEL_ERROR_VALUE -1
#endif

    class AFCThread
    {
    public:
        AFCThread();
        ~AFCThread();

        bool CreateThread(ThreadCallbackRun thread_callback_run, void* arg);

        int KillThread();

        bool IsAlive();

        ThreadID GetThreadID();

    private:
        ThreadID thread_id_;
    };
}

#endif
