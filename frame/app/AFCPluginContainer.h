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

#ifndef AFCPluginContainer_H
#define AFCPluginContainer_H

#include "base/AFPlatform.hpp"
#include "base/AFMap.hpp"
#include "interface/AFIPluginContainer.h"

namespace ark
{

    class AFIPluginManager;
    class AFCDynLib;
    class AFIPlugin;
    class AFIModule;
    class AFIApplication;

    class AFCPluginContainer : public AFIPluginContainer
    {
    public:
        AFCPluginContainer(AFIApplication* app, int logic_id, const std::string& plugin_path, const std::vector<std::string>& plugins);
        ~AFCPluginContainer() override;

        void Start() override;

    private:
        typedef void(*DLL_ENTRANCE_PLUGIN_FUNC)(AFIPluginManager* pm);
        typedef void(*DLL_EXIT_PLUGIN_FUNC)(AFIPluginManager* pm);

        std::map<std::string, bool> plugin_names_;
        std::vector<std::string> ordered_plugin_names_; // order
        AFMap<std::string, AFCDynLib> plugin_libs_;
        AFMap<std::string, AFIPlugin> plugin_instances_;
        AFMap<std::string, AFIModule> module_instances_;
        std::vector<AFIModule*> ordered_module_instances_; // order

    private:
        AFIApplication* app_{ nullptr };
        int logic_id_{ 0 };
    };

}

#endif //AFCPluginContainer_H
