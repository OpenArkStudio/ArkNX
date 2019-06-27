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

//#include "rapidxml/rapidxml.hpp"
//#include "rapidxml/rapidxml_iterators.hpp"
//#include "rapidxml/rapidxml_print.hpp"
//#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFXml.hpp"
#include "interface/AFIModule.h"
#include "AFCApplication.h"
#include "AFCPluginContainer.h"
#include "AFCLogicThreadManager.h"
#include "AFCThreadEventsManager.h"

namespace ark
{

    AFCApplication::AFCApplication()
    {
        cur_time_ = AFDateTime::GetNowTime();
        logic_thread_manager_ = ARK_NEW AFCLogicThreadManager();
        thread_event_manager_ = ARK_NEW AFCThreadEventsManager();

        logic_thread_manager_->Init(1000, thread_event_manager_);

        if (!LoadPluginConf())
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }

    AFCApplication::~AFCApplication()
    {
        //release all plugin containers.
        for (AFMap<int, AFIPluginContainer>::iterator iter = plugin_containers_.begin(); iter != plugin_containers_.end(); ++iter)
        {
            logic_thread_manager_->KillThread(iter->first);
            ARK_DELETE(iter->second);
        }

        ARK_DELETE(logic_thread_manager_);
        ARK_DELETE(thread_event_manager_);
    }

    bool AFCApplication::Start()
    {
        //Then create threads by configuration
        for (auto iter : thread_plugins_)
        {
            int thread_logic_id = iter.first;
            const std::vector<std::string>& thread_plugins = iter.second;
            //create a PluginContainer
            AFIPluginContainer* plugin_container = ARK_NEW AFCPluginContainer(this, thread_logic_id);
            //Start plugin container thread
            logic_thread_manager_->CreateThread(plugin_container);

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


    const std::string& AFCApplication::GetPluginPath()
    {
        return plugin_path_;
    }

    bool AFCApplication::GetPlugins(const int logic_id, std::vector<std::string>& plugins)
    {
        auto iter = thread_plugins_.find(logic_id);

        if (iter == thread_plugins_.end())
        {
            return false;
        }

        plugins = iter->second;
        return true;
    }

    bool AFCApplication::LoadPluginConf()
    {
        if (plugin_conf_path_.empty())
        {
            return true;
        }

        AFXml xml_doc(plugin_conf_path_);
        AFXmlNode root = xml_doc.GetRootNode();

        AFXmlNode plugins_node = root.FindNode("plugins");

        if (!plugins_node.IsValid())
        {
            return false;
        }

        plugin_path_ = plugins_node.GetString("path");
        int logic_index = 1;

        for (AFXmlNode thread_node = plugins_node.FindNode("thread"); thread_node.IsValid(); thread_node.NextNode())
        {
            std::map<int, std::vector<std::string>>::iterator iter = thread_plugins_.find(logic_index);

            if (iter == thread_plugins_.end())
            {
                std::vector<std::string> vec;
                thread_plugins_.insert(std::make_pair(logic_index, vec));
            }

            for (AFXmlNode plugin_node = thread_node.FindNode("plugin"); plugin_node.IsValid(); plugin_node.NextNode())
            {
                std::string plugin_name = plugin_node.GetString("name");
                std::string plugin_version = plugin_node.GetString("version");

                std::string plugin_full_name = std::string(plugin_name) + "@" + plugin_version;
                iter->second.emplace_back(plugin_full_name);

                //TODO:need order
            }

            ++logic_index;
        }

        return true;
    }

}