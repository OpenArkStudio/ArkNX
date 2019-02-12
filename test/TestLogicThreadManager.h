#ifndef _TEST_LOGICTHREADMANAGER_H
#define _TEST_LOGICTHREADMANAGER_H

#include "../frame/app/AFCLogicThreadManager.h"

namespace ark
{
    static void TestThreadInit(int logic_thread_id, AFIPluginManager* plugin_manager);

    static AFILogicThreadReturn TestThreadCallbackLogic(int logic_thread_id, AFIThreadEvent* thread_event, void* context);

    static ThreadError TestThreadErrorLogic(int logic_thread_id, ThreadLogicErrorType error_type, int& return_error, void* context);

    static void TestThreadExit(int logic_thread_id, AFIPluginManager* plugin_manager);

    void UnitTestLogicThreadManager(AFCLogicThreadManager& logic_thread_manager, AFIPluginManager* plugin_manager, AFIThreadEventManager* thread_event_manager);
}
#endif
