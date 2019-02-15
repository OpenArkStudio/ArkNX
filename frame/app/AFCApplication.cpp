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

namespace ark
{

    AFCApplication::AFCApplication()
    {
        cur_time_ = AFDateTime::GetNowTime();
    }

    bool AFCApplication::Start()
    {
        if (!LoadPluginConf())
        {
            return false;
        }

        //Then create threads by configuration
        //TODO:

        return true;
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
        //TODO:
        return true;
    }

}