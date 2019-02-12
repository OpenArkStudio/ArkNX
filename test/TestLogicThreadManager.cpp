#include "TestLogicThreadManager.h"

namespace ark
{

    void TestThreadInit(int logic_thread_id, AFIPluginManager* plugin_manager)
    {
        printf_s("[ThreadInit]logic_thread_id=%d.\n", logic_thread_id);
    }

    AFILogicThreadReturn TestThreadCallbackLogic(int logic_thread_id, AFIThreadEvent* thread_event, void* context)
    {
        AFILogicThreadReturn logic_thread_return;

        logic_thread_return.pause_time_ = 500;

        if (NULL != thread_event)
        {
            printf_s("[ThreadCallbackLogic]thread_event_id=%d.\n", thread_event->GetEventID());
        }

        printf_s("[ThreadCallbackLogic]thread_event=%d.\n", logic_thread_id);
        return logic_thread_return;
    }

    ThreadError TestThreadErrorLogic(int logic_thread_id, ThreadLogicErrorType error_type, int& return_error, void* context)
    {
        printf_s("[ThreadErrorLogic]thread_event=%d.\n", logic_thread_id);
        return ARK_THREAD_ERROR_CLOSE;
    }

    void TestThreadExit(int logic_thread_id, AFIPluginManager* plugin_manager)
    {
        printf_s("[ThreadExit]thread_event=%d.\n", logic_thread_id);
    }

    void UnitTestLogicThreadManager(AFCLogicThreadManager& logic_thread_manager, AFIPluginManager* plugin_manager, AFIThreadEventManager* thread_event_manager)
    {
        logic_thread_manager.Init(1000, plugin_manager, thread_event_manager);

        if (false == logic_thread_manager.CreateThread(1,
                TestThreadInit,
                TestThreadCallbackLogic,
                TestThreadErrorLogic,
                TestThreadExit,
                NULL))
        {
            printf_s("[UnitTestLogicThreadManager]Create thread error.\n");
            return;
        }

        printf_s("[UnitTestLogicThreadManager]Create thread OK.\n");
    }

}

