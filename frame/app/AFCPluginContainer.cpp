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

#include "interface/AFIApplication.h"
#include "interface/AFIModule.h"
#include "base/AFDateTime.hpp"
#include "AFCDynLib.h"
#include "AFCPluginContainer.h"
#include "interface/AFIPlugin.h"

namespace ark
{

    AFCPluginContainer::AFCPluginContainer(AFIApplication* app, int logic_id) :
        app_(app),
        logic_id_(logic_id)
    {
        ARK_ASSERT_RET_NONE(app != nullptr);
        //get all plug-ins
        ARK_ASSERT_RET_NONE(app->GetPlugins(logic_id, plugin_names_) == true);
        plugin_path_ = app->GetPluginPath();
    }

    AFCPluginContainer::~AFCPluginContainer()
    {

    }

    bool AFCPluginContainer::Init()
    {
        //load all dll/so in this thread
        for (const auto& iter : plugin_names_)
        {
            bool ret = LoadPluginLibrary(iter);
            ARK_ASSERT_RET_VAL(ret == true, false);
        }

        for (const auto& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            if (pModule)
            {
                pModule->Init();
            }
        }

        ////For now, DO NOT use
        //for (const auto& iter : module_instances_)
        //{
        //    AFIModule* pModule = iter.second;
        //    if (pModule)
        //    {
        //        pModule->PostInit();
        //    }
        //}

        //for (const auto& iter : module_instances_)
        //{
        //    AFIModule* pModule = iter.second;
        //    if (pModule)
        //    {
        //        pModule->CheckConfig();
        //    }
        //}

        //for (const auto& iter : module_instances_)
        //{
        //    AFIModule* pModule = iter.second;
        //    if (pModule)
        //    {
        //        pModule->PreUpdate();
        //    }
        //}

        return true;
    }

    bool AFCPluginContainer::Update()
    {
        //TODO:
        int64_t cur_time_ = AFDateTime::GetNowTime();

        for (const auto& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            if (pModule)
            {
                pModule->Update();
            }
        }

        return true;
    }

    bool AFCPluginContainer::Error()
    {
        //Do something
        //e.g. log or some special things
        return true;
    }

    bool AFCPluginContainer::Exit()
    {
        for (const auto& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            if (pModule)
            {
                pModule->PreShut();
            }
        }

        for (const auto& iter : module_instances_)
        {
            AFIModule* pModule = iter.second;
            if (pModule)
            {
                pModule->Shut();
            }
        }

        for (auto iter : plugin_names_)
        {
            UnloadPluginLibrary(iter);
        }

        module_instances_.clear();
        plugin_libs_.clear();
        plugin_instances_.clear();
        plugin_names_.clear();
        return true;
    }

    bool AFCPluginContainer::LoadPluginLibrary(const std::string& plugin_name)
    {
        AFCDynLib* pDynLib = plugin_libs_.find_value(plugin_name);
        if (pDynLib != nullptr)
        {
            return false;
        }

        AFCDynLib* pLib = ARK_NEW AFCDynLib(plugin_name);
        bool bLoad = pLib->Load(plugin_path_);
        if (bLoad)
        {
            plugin_libs_.insert(plugin_name, pLib);
            DLL_ENTRANCE_PLUGIN_FUNC pFunc = (DLL_ENTRANCE_PLUGIN_FUNC)pLib->GetSymbol("DllEntryPlugin");
            if (pFunc == nullptr)
            {
                CONSOLE_LOG << "Find function DllEntryPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
                assert(0);
                return false;
            }

            pFunc(this);

            return true;
        }
        else
        {
#if ARK_PLATFORM == PLATFORM_UNIX
            char* error = dlerror();
            if (error)
            {
                CONSOLE_LOG << stderr << " Load shared library[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
                CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
                assert(0);
                return false;
            }

#elif ARK_PLATFORM == PLATFORM_WIN
            CONSOLE_LOG << "Load dynamic library[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
            CONSOLE_LOG << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
#endif // ARK_PLATFORM
        }

        return true;
    }

    bool AFCPluginContainer::UnloadPluginLibrary(const std::string& plugin_name)
    {
        AFCDynLib* pDynLib = plugin_libs_.find_value(plugin_name);
        if (pDynLib == nullptr)
        {
            return false;
        }

        DLL_EXIT_PLUGIN_FUNC pFunc = (DLL_EXIT_PLUGIN_FUNC)pDynLib->GetSymbol("DllExitPlugin");
        if (pFunc != nullptr)
        {
            pFunc(this);
        }

        pDynLib->UnLoad();

        ARK_DELETE(pDynLib);
        plugin_libs_.erase(plugin_name);
        return true;
    }

    void AFCPluginContainer::Register(AFIPlugin* plugin)
    {
        std::string plugin_name = plugin->GetPluginName();

        if (!FindPlugin(plugin_name))
        {
            plugin->SetPluginContainer(this);
            plugin_instances_.insert(plugin_name, plugin);
            plugin->Install();
        }
        else
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }

    void AFCPluginContainer::Deregister(AFIPlugin* plugin)
    {
        ARK_ASSERT_RET_NONE(plugin != nullptr);

        auto find_plugin = FindPlugin(plugin->GetPluginName());
        if (find_plugin == nullptr)
        {
            return;
        }

        plugin->Uninstall();
        plugin_instances_.erase(plugin->GetPluginName());
        ARK_DELETE(plugin);
    }

    AFIPlugin* AFCPluginContainer::FindPlugin(const std::string& plugin_name)
    {
        return plugin_instances_.find(plugin_name)->second;
    }

    void AFCPluginContainer::AddModule(const std::string& module_name, AFIModule* module_ptr)
    {
        ARK_ASSERT_RET_NONE(FindModule(module_name) == nullptr);
        module_instances_.insert(module_name, module_ptr);
    }

    void AFCPluginContainer::RemoveModule(const std::string& module_name)
    {
        auto iter = module_instances_.find(module_name);
        if (iter == module_instances_.end())
        {
            return;
        }

        auto module_ptr = iter->second;
        module_instances_.erase(module_name);
    }

    AFIModule* AFCPluginContainer::FindModule(const std::string& module_name)
    {
        auto iter = module_instances_.find(module_name);
        if (iter != module_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

}