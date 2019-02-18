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
        int logic_thread_pause_timeout = 0;
        vector<AFIThreadEvent*> thread_events_list;

        AFCThreadParam* thread_param = (AFCThreadParam*)arg;

        //Init thread func
        thread_param->thread_init_(thread_param->thread_->GetThreadLogicID(),
                                   thread_param->thread_->GetManager()->GetPlugInManager());

        while (ARK_THREAD_STATE_LOGIC_CLOSE != thread_param->thread_->GetThreadState())
        {
            if (ARK_THREAD_STATE_LOGIC_PAUSE == thread_param->thread_->GetThreadState())
            {
                thread_param->thread_->SetCond(logic_thread_pause_timeout);
            }

            thread_param->thread_->Lock();

            int error_id = 0;
            thread_param->thread_->SaveLastRunTimeBegin();

            thread_events_list.clear();

            //get thread event type
            if (ARK_THREAD_EVENT_GET_SINGLE == thread_param->thread_->GetThreadEventGetType())
            {
                AFIThreadEvent* thread_event = thread_param->thread_->GetManager()->GetThreadEventManager()->GetEvent(thread_param->thread_->GetThreadLogicID());

                if (NULL != thread_event)
                {
                    thread_events_list.push_back(thread_event);
                }
            }
            else
            {
                thread_param->thread_->GetManager()->GetThreadEventManager()->GetEvents(thread_param->thread_->GetThreadLogicID(),
                        thread_events_list);
            }


            AFILogicThreadReturn thread_return = thread_param->thread_callback_logic_(thread_param->thread_->GetThreadLogicID(),
                                                 thread_events_list,
                                                 thread_param->thread_->GetManager(),
                                                 thread_param->arg_);

            thread_param->thread_->SaveLastRunTimeEnd();

            //release
            for (int i = 0; i < (int)thread_events_list.size(); i++)
            {
                if (NULL != thread_events_list[i])
                {
                    delete thread_events_list[i];
                }
            }

            if (ARK_THREAD_RETURN_ONCE == thread_return.thread_return_)
            {
                //thread logic just run once
                break;
            }
            else if (ARK_THREAD_RETURN_ERROR == thread_return.thread_return_)
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
                //need pause
                if (ARK_THREAD_RETURN_PAUSE == thread_return.thread_return_)
                {
                    thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_PAUSE);
                    logic_thread_pause_timeout = thread_return.pause_time_;
                }
                else
                {
                    thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_RUN_END);
                }

                thread_param->thread_->UnLock();
            }
        }

        thread_param->thread_->UnLock();
        thread_param->thread_exit_(thread_param->thread_->GetThreadLogicID(),
                                   thread_param->thread_->GetManager()->GetPlugInManager());


        thread_param->thread_->SetThreadState(ARK_THREAD_STATE_LOGIC_FINISH);
        return 0;
    };

    AFCThread::AFCThread() :
        thread_logic_id_(0),
        thread_id_(HANDEL_ERROR_VALUE),
        thread_mutex_(NULL),
        thread_cond_(NULL),
        thread_state_(ARK_THREAD_STATE_NONE),
        thread_event_get_type_(ARK_THREAD_EVENT_GET_SINGLE)
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
                                 ThreadEventGetType thread_event_get_type,
                                 ThreadInit thread_init,
                                 ThreadCallbackLogic thread_callback_logic,
                                 ThreadErrorLogic thread_callback_error,
                                 ThreadExit thread_exit,
                                 void* arg,
                                 AFIPluginManager* plugin_manager,
                                 AFIThreadEventsManager* event_manager,
                                 AFILogicThreadManager* logic_manager)
    {
        if (ARK_THREAD_STATE_NONE != thread_state_)
        {
            return false;
        }

        thread_param_.thread_ = (AFIThread*)this;
        thread_param_.arg_ = arg;
        thread_param_.thread_callback_logic_ = thread_callback_logic;
        thread_param_.thread_error_logic_ = thread_callback_error;
        thread_param_.thread_exit_ = thread_exit;
        thread_param_.thread_init_ = thread_init;

        thread_state_          = ARK_THREAD_STATE_INIT;
        thread_logic_id_       = thread_logic_id;
        thread_error_logic_    = thread_callback_error;
        thread_event_get_type_ = thread_event_get_type;

        manager_.Init(plugin_manager, event_manager, logic_manager);

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
        Lock();
        SetThreadState(ARK_THREAD_STATE_LOGIC_PAUSE);
        UnLock();
        return true;
    }

    bool AFCThread::Resume()
    {
        if (ARK_THREAD_STATE_LOGIC_PAUSE == GetThreadState())
        {
            UnLock();
        }

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

    AFIManager* AFCThread::GetManager()
    {
        return &manager_;
    }

    void AFCThread::SetCond(int interval_timeout)
    {
        if (0 == interval_timeout)
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
        else
        {
#ifdef WIN32
            SleepConditionVariableCS(reinterpret_cast<PCONDITION_VARIABLE>(thread_cond_),
                                     reinterpret_cast<PCRITICAL_SECTION>(thread_mutex_),
                                     interval_timeout);
#else
            struct timespec outtime;

            struct timeval now;
            struct timeval Interval;
            struct timeval abstime;

            Interval.tv_sec = (int)(interval_timeout / 1000);
            Interval.tv_usec = (interval_timeout % 1000) * 1000;

            gettimeofday(&now, NULL);
            timeradd(&now, &Interval, &abstime);

            outtime.tv_sec = abstime.tv_sec;
            outtime.tv_nsec = abstime.tv_usec * 1000;

            pthread_cond_timedwait(thread_cond_,
                                   thread_mutex_,
                                   &outtime);
#endif
        }
    }

    ThreadEventGetType AFCThread::GetThreadEventGetType()
    {
        return thread_event_get_type_;
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