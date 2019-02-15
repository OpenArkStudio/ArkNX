#pragma once

#include "base/AFPlatform.hpp"
#include "base/AFMap.hpp"

class AFIPluginManager;
class AFCDynLib;
class AFIPlugin;
class AFIModule;

class AFCPluginContainer
{
public:
protected:
private:
    typedef void(*DLL_ENTRANCE_PLUGIN_FUNC)(AFIPluginManager* pm);
    typedef void(*DLL_EXIT_PLUGIN_FUNC)(AFIPluginManager* pm);

    std::map<std::string, bool> plugin_names_;
    std::vector<std::string> ordered_plugin_names_; // order
    AFMap<std::string, AFCDynLib> plugin_libs_;
    AFMap<std::string, AFIPlugin> plugin_instances_;
    AFMap<std::string, AFIModule> module_instances_;
    std::vector<AFIModule*> ordered_module_instances_; // order
};