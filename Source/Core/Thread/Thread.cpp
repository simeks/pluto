#include "Common.h"

#include "Thread.h"

void SimpleThread::start(Runnable* runnable)
{
    _runnable = runnable;

    start_thread(run_runnable, this);
}

void SimpleThread::start(ThreadFunction thread_function, void* params)
{
    _thread_payload.function = thread_function;
    _thread_payload.params = params;

    start_thread(run_function, this);
}