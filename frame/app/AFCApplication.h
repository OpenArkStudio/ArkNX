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

#ifndef AFCApplication_H
#define AFCApplication_H

#include "base/AFPlatform.hpp"
#include "base/AFMap.hpp"
#include "interface/AFIApplication.h"
#include "interface/AFIPluginContainer.h"
#include "interface/AFILogicThreadManager.h"

namespace ark
{

    class AFIModule;

    class AFCApplication : public AFIApplication
    {
    public:
        AFCApplication();
        ~AFCApplication() override;

        virtual bool Start();
        virtual bool Stoped();

        virtual void RegModule(const std::string& module_uid, AFIModule* module_ptr);
        virtual void DeregModule(const std::string& module_uid);
        virtual AFIModule* FindModule(const std::string& module_uid);

        virtual const std::string& AppName() const;
        virtual void SetAppName(const std::string& app_name);

        virtual int64_t GetNowTime() const;

        virtual void SetPluginConfPath(const std::string& path);

        virtual const std::string& GetPluginPath();

        virtual bool GetPlugins(const int logic_id, std::vector<std::string>& plugins);

    protected:
        bool LoadPluginConf();

    private:
        int64_t cur_time_{ 0 };

        std::string app_name_{};        //application name
        std::string plugin_path_{};     //plugin dll/so directory
        std::string plugin_conf_path_{};//plugin config path

        std::map<int, std::vector<std::string>> thread_plugins_;
        AFMap<std::string, AFIModule> module_instances_;    //manage all modules
        AFMap<int, AFIPluginContainer> plugin_containers_;

        AFILogicThreadManager* logic_thread_manager_{ nullptr };
    };

}

#endif //AFCApplication_H