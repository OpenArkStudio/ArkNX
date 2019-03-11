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
#include "AFCPluginContainer.h"

namespace ark
{

    AFCPluginContainer::AFCPluginContainer(AFIApplication* app, int logic_id, const std::string& plugin_path, const std::vector<std::string>& plugins) :
        app_(app),
        logic_id_(logic_id)
    {
        //TODO:
    }

    AFCPluginContainer::~AFCPluginContainer()
    {
        //TODO:
    }

    void AFCPluginContainer::Start()
    {
        //TODO:
    }

}