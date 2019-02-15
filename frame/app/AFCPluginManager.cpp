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


#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFPlatform.hpp"
#include "base/AFDateTime.hpp"
#include "interface/AFIPlugin.h"
#include "AFCPluginManager.h"

namespace ark
{

    AFCPluginManager::AFCPluginManager() : AFIPluginManager()
    {
        cur_time_ = AFDateTime::GetNowTime();
    }

    bool AFCPluginManager::Init()
    {
        if (!LoadPluginConf())
        {
            return false;
        }

        for (const auto& iter : mxPluginNameVec)
        {
            bool bRet = LoadPluginLibrary(iter);

            if (!bRet)
            {
                return false;
            }
        }

        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Init();
            }
        }

        return true;
    }

    bool AFCPluginManager::LoadPluginConf()
    {
        if (plugin_conf_path_.empty())
        {
            return true;
        }

        rapidxml::file<> xml_doc(plugin_conf_path_.c_str());
        rapidxml::xml_document<>  doc;
        doc.parse<0>(xml_doc.data());

        rapidxml::xml_node<>* pRoot = doc.first_node();
        rapidxml::xml_node<>* pPluginsNode = pRoot->first_node("plugins");
        if (pPluginsNode == nullptr)
        {
            return false;
        }

        if (pPluginsNode->first_attribute("path") == nullptr)
        {
            ARK_ASSERT(0, "There are no ConfigPath.Name", __FILE__, __FUNCTION__);
            return false;
        }

        plugin_path_ = pPluginsNode->first_attribute("path")->value();

        ////load plugins which run in different thread
        //int thread_index = 0;
        //for (rapidxml::xml_node<>* pThread = pPluginsNode->first_node("thread"); pThread != nullptr; pThread = pThread->next_sibling("thread"))
        //{
        //    std::map<int, std::vector<std::string>>::iterator iter = thread_plugins_.find(thread_index);
        //    if (iter == thread_plugins_.end())
        //    {
        //        std::vector<std::string> vec;
        //        thread_plugins_.insert(std::make_pair(thread_index, vec));
        //    }

        //    for (rapidxml::xml_node<>* pPluginNode = pThread->first_node("plugin"); pPluginNode != nullptr; pPluginNode = pPluginNode->next_sibling("plugin"))
        //    {
        //        const char* plugin_name = pPluginNode->first_attribute("name")->value();
        //        iter->second.emplace_back(plugin_name);

        //        //TODO:need order
        //    }

        //    ++thread_index;
        //}

        rapidxml::xml_node<>* pResNode = pRoot->first_node("res");
        if (pResNode != NULL)
        {
            if (pResNode->first_attribute("path") == nullptr)
            {
                ARK_ASSERT(0, "There is no res.path", __FILE__, __FUNCTION__);
                return false;
            }

            res_path_ = pResNode->first_attribute("path")->value();
        }

        return true;
    }

    void AFCPluginManager::Register(AFIPlugin* plugin)
    {
        std::string strPluginName = plugin->GetPluginName();

        if (!FindPlugin(strPluginName))
        {
            plugin->SetPluginManager(this);
            mxPluginInstanceMap.insert(strPluginName, plugin);
            plugin->Install();
        }
        else
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }

    void AFCPluginManager::Deregister(AFIPlugin* plugin)
    {
        AFIPlugin* pPlugin = mxPluginInstanceMap.find_value(plugin->GetPluginName());

        if (pPlugin != nullptr)
        {
            pPlugin->Uninstall();
            mxPluginInstanceMap.erase(pPlugin->GetPluginName());
            ARK_DELETE(pPlugin);
        }
    }

    AFIPlugin* AFCPluginManager::FindPlugin(const std::string& strPluginName)
    {
        return mxPluginInstanceMap.find_value(strPluginName);
    }

    const std::string& AFCPluginManager::AppName() const
    {
        return app_name_;
    }

    inline int64_t AFCPluginManager::GetNowTime() const
    {
        return cur_time_;
    }

    inline const std::string& AFCPluginManager::GetResPath() const
    {
        return res_path_;
    }

    void AFCPluginManager::SetPluginConf(const std::string& strFileName)
    {
        if (strFileName.empty())
        {
            return;
        }

        if (strFileName.find(".plugin") == string::npos)
        {
            return;
        }

        plugin_conf_path_ = strFileName;
    }

    void AFCPluginManager::SetLogPath(const std::string& log_path)
    {
        log_path_ = log_path;
    }

    const std::string& AFCPluginManager::GetLogPath() const
    {
        return log_path_;
    }

    void AFCPluginManager::SetAppName(const std::string& app_name)
    {
        app_name_ = app_name;
    }

    void AFCPluginManager::AddModule(const std::string& strModuleName, AFIModule* pModule)
    {
        ARK_ASSERT_RET_NONE(FindModule(strModuleName) == nullptr);

        if (mxModuleInstanceMap.insert(strModuleName, pModule).second)
        {
            mxModuleInstanceVec.push_back(pModule);
        }
    }

    void AFCPluginManager::RemoveModule(const std::string& strModuleName)
    {
        auto pModule = mxModuleInstanceMap.find_value(strModuleName);
        mxModuleInstanceMap.erase(strModuleName);

        auto iter = std::find(mxModuleInstanceVec.begin(), mxModuleInstanceVec.end(), pModule);
        if (iter != mxModuleInstanceVec.end())
        {
            mxModuleInstanceVec.erase(iter);
        }
    }

    AFIModule* AFCPluginManager::FindModule(const std::string& strModuleName)
    {
        return mxModuleInstanceMap.find_value(strModuleName);
    }

    bool AFCPluginManager::PostInit()
    {
        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PostInit();
            }
        }

        return true;
    }

    bool AFCPluginManager::CheckConfig()
    {
        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->CheckConfig();
            }
        }

        return true;
    }

    bool AFCPluginManager::PreUpdate()
    {
        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PreUpdate();
            }
        }

        return true;
    }

    bool AFCPluginManager::Update()
    {
        cur_time_ = AFDateTime::GetNowTime();

        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Update();
            }
        }

        return true;
    }

    bool AFCPluginManager::PreShut()
    {
        cur_time_ = AFDateTime::GetNowTime();

        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->PreShut();
            }
        }

        return true;
    }

    bool AFCPluginManager::Shut()
    {
        for (const auto& iter : mxModuleInstanceVec)
        {
            AFIModule* pModule = iter;
            if (pModule)
            {
                pModule->Shut();
            }
        }

        for (auto it : mxPluginNameMap)
        {
            UnloadPluginLibrary(it.first);
        }

        mxPluginInstanceMap.clear();
        mxPluginNameMap.clear();
        return true;
    }

    bool AFCPluginManager::LoadPluginLibrary(const std::string& plugin_name)
    {
        AFCDynLib* pDynLib = mxPluginLibMap.find_value(plugin_name);
        if (pDynLib != nullptr)
        {
            return false;
        }

        AFCDynLib* pLib = ARK_NEW AFCDynLib(plugin_name);
        bool bLoad = pLib->Load(plugin_path_);
        if (bLoad)
        {
            mxPluginLibMap.insert(plugin_name, pLib);
            DLL_ENTRY_PLUGIN_FUNC pFunc = (DLL_ENTRY_PLUGIN_FUNC)pLib->GetSymbol("DllEntryPlugin");
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

    bool AFCPluginManager::UnloadPluginLibrary(const std::string& plugin_name)
    {
        AFCDynLib* pDynLib = mxPluginLibMap.find_value(plugin_name);
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
        mxPluginLibMap.erase(plugin_name);
        return true;
    }

}