// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "TaskScheduler.h"



//-------------------------------------------------------------------------------
TaskScheduler::TaskScheduler(int num_threads)
    : _next_task_id(0)
{
    _num_threads = num_threads;
    _shutting_down = false;
}
TaskScheduler::~TaskScheduler()
{
    if (!_workers.empty()) // Still initialized
        shutdown();
}
//-------------------------------------------------------------------------------
void TaskScheduler::initialize()
{
    _shutting_down = false;

    if (!_num_threads)
        _num_threads = _num_processors - 1; // -1 for the main thread
    if (_num_threads > MAX_WORKER_THREAD_COUNT)
        _num_threads = MAX_WORKER_THREAD_COUNT;

    _workers.resize(_num_threads);

    for (uint32_t i = 0; i < _num_threads; ++i)
    {
        _workers[i] = new WorkerThread(this);
        _workers[i]->start();
    }
}
void TaskScheduler::shutdown()
{
    _shutting_down = true;

    // Wait for workers to finish
    wait_for_workers();
    // Wake them and let them quit
    wake_workers();

    for (uint32_t i = 0; i < _num_threads; ++i)
    {
        while (_workers[i]->is_running())
        {
            Sleep(1);
        }
        delete _workers[i];
    }
    _workers.clear();
}
//-------------------------------------------------------------------------------
TaskId TaskScheduler::prepare_task(const WorkItem& work_item)
{
    TaskId task_id;

    // Allocate a new task from our memory pool
    Task* task = allocate_task(task_id);
    task->parent = INVALID_TASK_ID;
    task->num_work_items = 1;
    task->work_item.kernel = work_item.kernel;
    task->work_item.data = work_item.data;

    return task_id;
}
TaskId TaskScheduler::prepare_empty_task()
{
    TaskId task_id;

    // Allocate a new task from our memory pool
    Task* task = allocate_task(task_id);
    task->parent = INVALID_TASK_ID;
    task->num_work_items = 1;
    task->work_item.kernel = 0;
    task->work_item.data = 0;

    return task_id;
}
//-------------------------------------------------------------------------------
void TaskScheduler::set_parent(const TaskId& task_id, const TaskId& parent_id)
{
    Task* task = _task_pool.object(task_id.pool_handle);
    assert(task);
    task->parent = parent_id.pool_handle;
    assert(task_id.pool_handle != parent_id.pool_handle);

    Task* parent = _task_pool.object(parent_id.pool_handle);
    assert(parent);
    thread::interlocked_increment(&parent->num_work_items);
}
//-------------------------------------------------------------------------------
void TaskScheduler::spawn_task(const TaskId& task_id)
{
    Task* task = _task_pool.object(task_id.pool_handle);
    assert(task->id == task_id.id);
    push_task(task);
}
void TaskScheduler::spawn_task_and_wait(const TaskId& task_id)
{
    Task* wait_for_task = _task_pool.object(task_id.pool_handle);
    assert(wait_for_task->id == task_id.id);
    push_task(wait_for_task);

    while (!has_completed(task_id))
    {
        Task* task;
        if (pop_task(&task))
        {
            run_task(task);
        }
        else
        {
            // No work available, yield and let the other threads work
            Sleep(0);
        }
    }
}
//-------------------------------------------------------------------------------
void TaskScheduler::set_worker_count(uint32_t num)
{
    assert(num > MAX_WORKER_THREAD_COUNT);
    if (!num)
        num = _num_processors;

    if (num == _num_threads)
        return;

    // Restart the manager
    shutdown();
    _num_threads = num;
    initialize();
}
uint32_t TaskScheduler::worker_count() const
{
    return _num_threads;
}

//-------------------------------------------------------------------------------
TaskScheduler::Task* TaskScheduler::allocate_task(TaskId& task_id)
{
    Task* task = _task_pool.allocate();
    memset(task, 0, sizeof(Task));
    assert(task);
    task->id = thread::interlocked_increment(&_next_task_id);

    task_id.id = task->id;
    task_id.pool_handle = _task_pool.index(task);
    assert(is_valid<uint32_t>(task_id.pool_handle));

    return task;
}
void TaskScheduler::release_task(Task* task)
{
    task->parent = invalid<uint32_t>();
    task->id = thread::interlocked_increment(&_next_task_id);
    _task_pool.release(task);
}
//-------------------------------------------------------------------------------
void TaskScheduler::push_task(Task* task)
{
    bool run_now = false;

    // Run directly if we're single threaded
    if (worker_count() < 1)
    {
        run_now = true;
    }
    else
        // Push task to the queue 
    {
        ScopedLock<CriticalSection> scoped_lock(_task_lock);
        _task_queue.push_back(task);
    }

    wake_workers();

    // Run the task now if it wasn't added
    if (run_now)
    {
        run_task(task);
    }
}
//-------------------------------------------------------------------------------
void TaskScheduler::run_task(Task* task)
{
    if (task->work_item.kernel)
        task->work_item.kernel(task->work_item.data);

    complete_task(task);
}
//-------------------------------------------------------------------------------
void TaskScheduler::complete_task(Task* task)
{
    // The task is now complete so we decrement our number of workitems
    if (thread::interlocked_decrement(&task->num_work_items) == 0)
    {
        // Also notify the tasks parent
        if (is_valid(task->parent))
        {
            Task* parent = _task_pool.object(task->parent);
            assert(parent);
            complete_task(parent);
        }
        release_task(task);
    }
}
//-------------------------------------------------------------------------------
bool TaskScheduler::has_completed(const TaskId& task_id)
{
    Task* task = _task_pool.object(task_id.pool_handle);
    assert(task);
    if (task->id != task_id.id)
    {
        return true;
    }
    else if (task->num_work_items == 0)
    {
        return true;
    }

    return false;
}
//-------------------------------------------------------------------------------
bool TaskScheduler::pop_task(Task** task)
{
    ScopedLock<CriticalSection> scoped_lock(_task_lock);

    if (_task_queue.empty())
        return false;

    *task = _task_queue.back();
    _task_queue.pop_back();

    return true;
}
//-------------------------------------------------------------------------------
void TaskScheduler::wake_workers()
{
    for (uint32_t i = 0; i < _num_threads; ++i)
        _sem_thread_wakeup.set();
}
//-------------------------------------------------------------------------------
void TaskScheduler::wait_for_workers()
{
    // Wait for all workers to go to sleep
    for (uint32_t i = 0; i < _num_threads; ++i)
        _sem_thread_sleep.wait();
}
//-------------------------------------------------------------------------------
bool TaskScheduler::is_shutting_down() const
{
    return _shutting_down;
}

//------------------------------------------------------------------------------

namespace parallel_for_internal
{
    struct ParallelForTaskData
    {
        ParallelForKernel kernel;
        Range range;
        void* data;
    };

    void ParallelForTaskKernel(void* data)
    {
        ParallelForTaskData* parallelfor_data = (ParallelForTaskData*)data;
        parallelfor_data->kernel(parallelfor_data->data, parallelfor_data->range);
    }
}
void scheduling::parallel_for(TaskScheduler* scheduler, ParallelForKernel fn, void* param, const Range& range)
{
    parallel_for_internal::ParallelForTaskData task_data[MAX_WORKER_THREAD_COUNT + 1];

    uint32_t part_size, rest;

    uint32_t begin = range.begin;
    uint32_t end = range.end;

    uint32_t count = (end - begin);
    assert(count);

    // Spread the task so we have one task per thread
    count = min(count, scheduler->worker_count() + 1); // +1 as the main thread will perform work

    part_size = (end - begin) / count;
    rest = (end - begin) % count;

    // Define a empty task as a parent to all the sub-tasks
    TaskId parent_task = scheduler->prepare_empty_task();


    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t size = part_size + int(rest > i);

        task_data[i].range = Range(begin, begin + size);
        task_data[i].data = param;
        task_data[i].kernel = fn;

        WorkItem item;
        item.data = &task_data[i];
        item.kernel = parallel_for_internal::ParallelForTaskKernel;

        TaskId sub_task = scheduler->prepare_task(item);
        scheduler->set_parent(sub_task, parent_task);
        scheduler->spawn_task(sub_task);

        begin += size;
    }

    // Spawn and wait for the parent task to finish
    //  The parent task won't finish before all sub-tasks are completed
    scheduler->spawn_task_and_wait(parent_task);
}

//-------------------------------------------------------------------------------


