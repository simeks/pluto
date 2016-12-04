// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "TaskScheduler.h"


//-------------------------------------------------------------------------------
void TaskScheduler::WorkerThread::start()
{
    // Start a simple thread with this as a Runnable
    _thread.start(this);
}

// Runnable 
void TaskScheduler::WorkerThread::run()
{
    // Run
    while (1)
    {
        idle();

        Task* task;
        while (_owner->pop_task(&task))
        {
            // Run task
            _owner->run_task(task);
        }

        if (_owner->is_shutting_down())
            break;
    }
}
void TaskScheduler::WorkerThread::idle()
{
    // Notify that we are going to sleep
    _owner->_sem_thread_sleep.set();

    // Sleep
    _owner->_sem_thread_wakeup.wait();
}

int TaskScheduler::WorkerThread::get_worker_index()
{
    return int(this - _owner->_workers[0]);
}
//------------------------------------------------------------------------------

