#ifndef _AFITHREADWAKEUP_H
#define _AFITHREADWAKEUP_H

class AFIThreadWakeUp
{
public:
    AFIThreadWakeUp() {};

    virtual ~AFIThreadWakeUp() {};

    virtual bool Wakeup(int thread_logic_id) = 0;
};

#endif
