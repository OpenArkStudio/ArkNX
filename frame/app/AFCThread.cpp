#include "AFCThread.h"

namespace ark
{

    //run thread logic
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned __stdcall ThreadCallbackRun(void* arg)
#else
    void* ThreadCallbackRun(void* arg)
#endif
    {
        AFCThreadParam* thread_param = (AFCThreadParam*)arg;

        //Init thread func
        thread_param->thread_init_(thread_param->thread_->GetThreadLogicID(),
                                   thread_param->thread_->GetPluginManager());

        while (ARK_THREAD_STATE_LOGIC_CLOSE != thread_param->thread_->GetThreadState())
        {
            if (ARK_THREAD_STATE_LOGIC_PAUSE == thread_param->thread_->GetThreadState())
            {
                thread_param->thread_->SetCond();
            }

            thread_param->thread_->Lock();

            int error_id = 0;
            thread_param->thread_->SaveLastRunTimeBegin();
            AFIThreadEvent* thread_event = thread_param->thread_->GetThreadEvent();
            ThreadReturn thread_return = thread_param->thread_callback_logic_(thread_param->thread_->GetThreadLogicID(),
                                         thread_event,
                                         thread_param->arg_);

            thread_param->thread_->SaveLastRunTimeEnd();

            //release
            if (NULL != thread_event)
            {
                delete thread_event;
            }

            if (ARK_THREAD_RETURN_ONCE == thread_return)
            {
                //thread logic just run once
                break;
            }
            else if (ARK_THREAD_RETURN_ERROR == thread_return)
            {
                //call thread logic error function
                thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_ERROR);
                ThreadError thread_error = thread_param->thread_error_logic_(thread_param->thread_->GetThreadLogicID(),
                                           ARK_THREAD_LOGIC_ERROR,
                                           error_id,
                                           thread_param->arg_);

                if (ARK_THREAD_ERROR_CLOSE == thread_error)
                {
                    break;
                }
            }
            else
            {
                thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_RUN_END);
                thread_param->thread_->UnLock();
            }
        }

        thread_param->thread_->UnLock();
        thread_param->thread_exit_(thread_param->thread_->GetThreadLogicID(),
                                   thread_param->thread_->GetPluginManager());


        thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_FINISH);
        return 0;
    };

    AFCThread::AFCThread() :
        thread_logic_id_(0),
        thread_id_(HANDEL_ERROR_VALUE),
        thread_mutex_(NULL),
        thread_cond_(NULL),
        thread_state_(ARK_THREAD_STATE_NONE),
        plugin_manager_(NULL),
        event_manager_(NULL)
    {
#if ARK_PLATFORM == PLATFORM_WIN
        thread_mutex_ = new CRITICAL_SECTION();
        thread_cond_ = new CONDITION_VARIABLE();
        InitializeCriticalSection(thread_mutex_);
        InitializeConditionVariable(thread_cond_);
#else
        thread_mutex_ = new pthread_mutex_t();
        thread_cond_ = new pthread_cond_t();
        pthread_mutex_init(thread_mutex_, NULL);
        pthread_cond_init(thread_cond_, NULL);
#endif
    }

    AFCThread::~AFCThread()
    {
        if (NULL != thread_mutex_)
        {
            delete thread_mutex_;
            thread_mutex_ = NULL;
        }

        if (NULL != thread_cond_)
        {
            delete thread_cond_;
            thread_cond_ = NULL;
        }
    }

    bool AFCThread::CreateThread(int thread_logic_id,
                                 ThreadInit thread_init,
                                 ThreadCallbackLogic thread_callback_logic,
                                 ThreadErrorLogic thread_callback_error,
                                 ThreadExit thread_exit,
                                 void* arg,
                                 AFIPluginManager* plugin_manager,
                                 AFIThreadEventManager* event_manager)
    {
        if (ARK_THREAD_STATE_NONE != thread_state_)
        {
            return false;
        }

        thread_param_.thread_                = (AFIThread*)this;
        thread_param_.arg_                   = arg;
        thread_param_.thread_callback_logic_ = thread_callback_logic;
        thread_param_.thread_error_logic_    = thread_callback_error;
        thread_param_.thread_exit_           = thread_exit;
        thread_param_.thread_init_           = thread_init;

        thread_state_                        = ARK_THREAD_STATE_INIT;
        thread_logic_id_                     = thread_logic_id;
        thread_error_logic_                  = thread_callback_error;
        plugin_manager_                      = plugin_manager;
        event_manager_                       = event_manager;

#if ARK_PLATFORM == PLATFORM_WIN
        unsigned int thread_id = 0;
        thread_id_ = (HANDLE)_beginthreadex(NULL, 0, ThreadCallbackRun, (PVOID)&thread_param_, 0, &thread_id);
#else
        pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)&arg);
#endif

        if (thread_id_ != HANDEL_ERROR_VALUE)
        {
            create_thread_time_.update();
            return true;
        }
        else
        {
            return false;
        }
    }

    int AFCThread::KillThread()
    {
        if (HANDEL_ERROR_VALUE != thread_id_)
        {
#if ARK_PLATFORM == PLATFORM_WIN
            CloseHandle(thread_id_);
            thread_id_ = HANDEL_ERROR_VALUE;
#else
            pthread_cancel(thread_id_);
#endif
        }

        return 0;
    }

    void AFCThread::StopThread()
    {
        thread_state_ = ARK_THREAD_STATE_LOGIC_CLOSE;
    }

    bool AFCThread::IsAlive()
    {
        if (HANDEL_ERROR_VALUE != thread_id_)
        {
#if ARK_PLATFORM == PLATFORM_WIN
            return true;
#else
            int kill_rc = pthread_kill(thread_id_, 0);

            if (kill_rc == ESRCH)
            {
                thread_id_ = HANDEL_ERROR_VALUE;
                return false;
            }
            else
            {
                return true;
            }

#endif
        }
        else
        {
            return false;
        }
    }

    bool AFCThread::Pause()
    {
        SetThreadState(ARK_THREAD_STATE_LOGIC_PAUSE);
        return true;
    }

    bool AFCThread::Resume()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        LeaveCriticalSection(thread_mutex_);
#else
        pthread_mutex_unlock(thread_mutex_);
#endif
        return true;
    }

    void AFCThread::Lock()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        EnterCriticalSection(thread_mutex_);
#else
        pthread_mutex_lock(thread_mutex_);
#endif
    }

    void AFCThread::UnLock()
    {
#ifdef WIN32
        LeaveCriticalSection(thread_mutex_);
#else
        pthread_mutex_unlock(thread_mutex_);
#endif
    }

    void AFCThread::SaveLastRunTimeBegin()
    {
        //save thread last run time begin
        SetThreadState(ARK_THREAD_STATE_LOGIC_RUN_BEGIN);
        logic_begin_thread_time_.update();
    }

    void AFCThread::SaveLastRunTimeEnd()
    {
        //save thread last run time End
        if (ARK_THREAD_STATE_LOGIC_PAUSE != GetThreadState())
        {
            SetThreadState(ARK_THREAD_STATE_LOGIC_RUN_END);
        }

        logic_end_thread_time_.update();
    }

    AFDateTime AFCThread::GetCreatehreadTime()
    {
        return create_thread_time_;
    }

    AFDateTime AFCThread::GetLogicBeginThreadTime()
    {
        return logic_begin_thread_time_;
    }

    AFDateTime AFCThread::GetLogicEndThreadTime()
    {
        return logic_end_thread_time_;
    }

    ThreadID AFCThread::GetThreadID()
    {
        return thread_id_;
    }

    void AFCThread::ThreadTimeoutCallBack()
    {
        int error_id = 0;
        thread_error_logic_(thread_logic_id_,
                            ARK_THREAD_LOGIC_TIMEOUT,
                            error_id,
                            NULL);
    }

    ThreadState AFCThread::GetThreadState()
    {
        return thread_state_;
    }

    AFIPluginManager* AFCThread::GetPluginManager()
    {
        return plugin_manager_;
    }

    ark::AFIThreadEvent* AFCThread::GetThreadEvent()
    {
        return event_manager_->GetEvent(thread_logic_id_);
    }

    void AFCThread::SetCond()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        SleepConditionVariableCS(reinterpret_cast<PCONDITION_VARIABLE>(thread_cond_),
                                 reinterpret_cast<PCRITICAL_SECTION>(thread_mutex_),
                                 INFINITE);
#else
        pthread_cond_wait(thread_cond_,
                          thread_mutex_);
#endif
    }

    int AFCThread::GetThreadLogicID()
    {
        return thread_logic_id_;
    }

    void AFCThread::SetThreadState(ThreadState thread_state)
    {
        thread_state_ = thread_state;
    }

}