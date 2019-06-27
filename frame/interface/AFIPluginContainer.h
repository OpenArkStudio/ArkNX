/*
* This source file is part of ArkNX
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

#ifndef AFIPluginContainer_H
#define AFIPluginContainer_H

#include "interface/AFIModule.h"

namespace ark
{
    //Thread return
    enum ThreadReturn
    {
        ARK_THREAD_RETURN_CONTINUE = 0,
        ARK_THREAD_RETURN_ONCE,
        ARK_THREAD_RETURN_ERROR,
        ARK_THREAD_RETURN_PAUSE,
    };

    class AFILogicThreadReturn
    {
    public:
        AFILogicThreadReturn() : thread_return_(ARK_THREAD_RETURN_CONTINUE), pause_time_(0)
        {

        }

        ThreadReturn thread_return_;
        int          pause_time_;
    };

#define ARK_DLL_PLUGIN_ENTRY(plugin_name)               \
    ARK_EXPORT void DllEntryPlugin(AFIPluginContainer* p)   \
    {                                                       \
        p->Register<plugin_name>();                         \
    }

#define ARK_DLL_PLUGIN_EXIT(plugin_name)                \
    ARK_EXPORT void DllExitPlugin(AFIPluginContainer* p)    \
    {                                                       \
        p->Deregister<plugin_name>();                       \
    }

    class AFIPlugin;

    class AFIPluginContainer
    {
    public:
        virtual ~AFIPluginContainer() {}

        virtual bool Init() = 0;
        virtual AFILogicThreadReturn Update() = 0;
        virtual bool Error(int err_id) = 0;
        virtual bool Exit() = 0;

        virtual std::string GetParam(const char* pName) = 0;

        template <typename T>
        T* FindModule()
        {
            AFIModule* pLogicModule = FindModule(typeid(T).name());
            ARK_ASSERT_RET_VAL(pLogicModule != nullptr, nullptr);

            if (!std::is_base_of<AFIModule, T>::value)
            {
                return nullptr;
            }

            T* pT = dynamic_cast<T*>(pLogicModule);
            ARK_ASSERT_RET_VAL(pT != nullptr, nullptr);

            return pT;
        }

        template<typename PLUGIN_TYPE>
        void Register()
        {
            AFIPlugin* pNewPlugin = ARK_NEW PLUGIN_TYPE();
            Register(pNewPlugin);
        }

        template<typename PLUGIN_TYPE>
        void Deregister()
        {
            Deregister(FindPlugin(typeid(PLUGIN_TYPE).name()));
        }

        virtual void Register(AFIPlugin* plugin) = 0;
        virtual void Deregister(AFIPlugin* plugin) = 0;
        virtual AFIPlugin* FindPlugin(const std::string& plugin_name) = 0;

        virtual void AddModule(const std::string& module_name, AFIModule* pModule) = 0;
        virtual void RemoveModule(const std::string& module_name) = 0;
        virtual AFIModule* FindModule(const std::string& module_name) = 0;

        virtual AFIApplication* GetApplication() = 0;
    };

}

#endif //AFIPluginContainer_H