#ifndef _TEST_LOGICTHREADMANAGER_H
#define _TEST_LOGICTHREADMANAGER_H

#include "../frame/app/AFCLogicThreadManager.h"

namespace ark
{
    static void TestThreadInit(int logic_thread_id, AFIPluginContainer* plugin_manager);

    static AFILogicThreadReturn TestThreadCallbackLogic(int logic_thread_id, std::vector<AFIThreadEvent*>& thread_events_list, AFIManager* manager, void* context);

    static ThreadError TestThreadErrorLogic(int logic_thread_id, ThreadLogicErrorType error_type, int& return_error, void* context);

    static void TestThreadExit(int logic_thread_id, AFIPluginContainer* plugin_manager);

    void UnitTestLogicThreadManager(AFCLogicThreadManager& logic_thread_manager, AFIPluginContainer* plugin_manager, AFIThreadEventsManager* thread_event_manager);
}
#endif
