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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFDateTime.hpp"
#include "interface/AFIModule.h"
#include "AFCApplication.h"
#include "AFCPluginContainer.h"

namespace ark
{

    AFCApplication::AFCApplication()
    {
        cur_time_ = AFDateTime::GetNowTime();

        if (!LoadPluginConf())
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }


    AFCApplication::~AFCApplication()
    {
        //release all plugin containers.
        for (auto iter : plugin_containers_)
        {
            ARK_DELETE(iter.second);
        }
    }

    bool AFCApplication::Start()
    {
        //Then create threads by configuration
        for (auto iter : thread_plugins_)
        {
            int thread_logic_id = iter.first;
            const std::vector<std::string>& thread_plugins = iter.second;
            //create a PluginContainer
            AFIPluginContainer* plugin_container = ARK_NEW AFCPluginContainer(this, thread_logic_id, plugin_path_, thread_plugins);
            //Start plugin container
            plugin_container->Start();
            //manage
            plugin_containers_.insert(thread_logic_id, plugin_container);
        }

        return true;
    }


    bool AFCApplication::Stoped()
    {
        //TODO:
        return false;
    }

    void AFCApplication::RegModule(const std::string& module_uid, AFIModule* module_ptr)
    {
        ARK_ASSERT_RET_NONE(module_ptr != nullptr);
        ARK_ASSERT_RET_NONE(module_instances_.insert(module_uid, module_ptr).second == true);
    }

    void AFCApplication::DeregModule(const std::string& module_uid)
    {
        module_instances_.erase(module_uid);
    }

    ark::AFIModule* AFCApplication::FindModule(const std::string& module_uid)
    {
        return module_instances_.find_value(module_uid);
    }

    const std::string& AFCApplication::AppName() const
    {
        return app_name_;
    }

    void AFCApplication::SetAppName(const std::string& app_name)
    {
        app_name_ = app_name;
    }

    int64_t AFCApplication::GetNowTime() const
    {
        return cur_time_;
    }

    void AFCApplication::SetPluginConfPath(const std::string& path)
    {
        plugin_conf_path_ = path;
    }

    bool AFCApplication::LoadPluginConf()
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

        //load plugins which run in different thread
        int logic_index = 1;
        for (rapidxml::xml_node<>* pThread = pPluginsNode->first_node("thread"); pThread != nullptr; pThread = pThread->next_sibling("thread"))
        {
            std::map<int, std::vector<std::string>>::iterator iter = thread_plugins_.find(logic_index);
            if (iter == thread_plugins_.end())
            {
                std::vector<std::string> vec;
                thread_plugins_.insert(std::make_pair(logic_index, vec));
            }

            for (rapidxml::xml_node<>* pPluginNode = pThread->first_node("plugin"); pPluginNode != nullptr; pPluginNode = pPluginNode->next_sibling("plugin"))
            {
                const char* plugin_name = pPluginNode->first_attribute("name")->value();
                iter->second.emplace_back(plugin_name);

                //TODO:need order
            }

            ++logic_index;
        }

        return true;
    }

}