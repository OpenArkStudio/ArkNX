#include "AFCThread.h"


ark::AFCThread::AFCThread() : thread_id_(-1)
{

}

ark::AFCThread::~AFCThread()
{

}

bool ark::AFCThread::CreateThread(ThreadCallbackRun, void* arg)
{
#if ARK_PLATFORM == PLATFORM_WIN
    CreateThread(NULL, 0, ThreadCallbackRun, (PVOID)arg, 0, &thread_id_);
#else
    pthread_create(&thread_id_, NULL, ThreadCallbackRun, (void*)&arg);
    m_TimerInfoList.Set_Thread_ID(nThreadID);
#endif
}
