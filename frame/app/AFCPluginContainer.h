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

    class AFCDynLib;
    class AFIPlugin;
    class AFIModule;
    class AFIApplication;

    class AFCPluginContainer : public AFIPluginContainer
    {
    public:
        AFCPluginContainer(AFIApplication* app, int logic_id);

        virtual bool Init();
        virtual AFILogicThreadReturn  Update();
        virtual bool Error(int err_id);
        virtual bool Exit();

        virtual void Register(AFIPlugin* pPlugin);
        virtual void Deregister(AFIPlugin* pPlugin);

        virtual std::string GetParam(const char* pName);

        virtual AFIPlugin* FindPlugin(const std::string& module_name);

        virtual void AddModule(const std::string& module_name, AFIModule* pModule);
        virtual void RemoveModule(const std::string& module_name);
        virtual AFIModule* FindModule(const std::string& module_name);

        virtual AFIApplication* GetApplication();

    protected:
        bool LoadPluginLibrary(const std::string& plugin_name, const std::string& plugin_version);
        bool UnloadPluginLibrary(const std::string& plugin_name);

        const std::string& FindPluginVersion(const std::string& plugin_name);

    private:
        typedef void(*DLL_ENTRANCE_PLUGIN_FUNC)(AFIPluginContainer*);
        typedef void(*DLL_EXIT_PLUGIN_FUNC)(AFIPluginContainer*);

        std::vector<std::string> plugin_names_;
        AFMap<std::string, AFCDynLib> plugin_libs_;
        std::map<std::string, std::string> plugin_vers_;
        AFMap<std::string, AFIPlugin> plugin_instances_;
        AFMap<std::string, AFIModule> module_instances_;

    private:
        AFIApplication* app_;
        int logic_id_;
        std::string plugin_path_;
    };

}

#endif //AFCPluginContainer_H
