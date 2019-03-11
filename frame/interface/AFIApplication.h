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

#ifndef AFIApplication_H
#define AFIApplication_H

#include "base/AFPlatform.hpp"

namespace ark
{

    class AFIModule;

    class AFIApplication
    {
    public:
        AFIApplication() = default;
        virtual ~AFIApplication() = default;

        template <typename T>
        T* FindModule()
        {
            AFIModule* logic_module = FindModule(typeid(T).name());
            ARK_ASSERT_RET_VAL(logic_module != nullptr, nullptr);

            if (!std::is_base_of<AFIModule, T>::value)
            {
                return nullptr;
            }

            T* derived_module = dynamic_cast<T*>(logic_module);
            ARK_ASSERT_RET_VAL(derived_module != nullptr, nullptr);

            return derived_module;
        }

        virtual bool Start() = 0;
        virtual bool Stoped() = 0;

        virtual void RegModule(const std::string& module_uid, AFIModule* module_ptr) = 0;
        virtual void DeregModule(const std::string& module_uid) = 0;
        virtual AFIModule* FindModule(const std::string& module_uid) = 0;

        virtual const std::string& AppName() const = 0;
        virtual void SetAppName(const std::string& app_name) = 0;

        virtual int64_t GetNowTime() const = 0;

        virtual void SetPluginConfPath(const std::string& path) = 0;

        virtual const std::string& GetPluginPath() = 0;

        virtual bool GetPlugins(const int logic_id, std::vector<std::string>& plugins) = 0;
    };

}

#endif //AFIApplication_H