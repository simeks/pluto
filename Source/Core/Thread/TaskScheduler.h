// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_TASKSCHEDULER_H__
#define __CORE_TASKSCHEDULER_H__

#include "Thread.h"
#include "Memory/MemoryPool.h"

/// Maximum total number of worker threads
#define MAX_WORKER_THREAD_COUNT 7
#define TASK_POOL_SIZE (256 * MAX_WORKER_THREAD_COUNT) 
#define INVALID_TASK_ID 0xffffffff

struct Range
{
    int begin, end;
    Range(int b = 0, int e = 0) : begin(b), end(e)
    {
    }
};

typedef void(*TaskKernel)(void*);
typedef void(*ParallelForKernel)(void*, const Range&);


struct TaskId
{
    uint32_t id; // Unique identifier for a task.
    uint32_t pool_handle; // Handle to a task in the task pool.
};

/// This is basicly what a task is, a callback and a raw pointer for data as a 
/// parameter for the callback.
struct WorkItem
{
    TaskKernel kernel;
    void* data;
};


/// @brief Task scheduler
class TaskScheduler
{
public:
    TaskScheduler(int num_threads = MAX_WORKER_THREAD_COUNT);
    ~TaskScheduler();

    //-------------------------------------------------------------------------------

    /// @brief Initializes the scheduler
    void initialize();

    /// @brief Shuts the scheduler down
    void shutdown();

    //-------------------------------------------------------------------------------

    /// @brief Prepares a new task with the specified WorkItem to be run.
    TaskId prepare_task(const WorkItem& work_item);

    /// @brief Prepares an empty task.
    ///
    /// This will work as a any other task but will not do any real work when spawned.
    TaskId prepare_empty_task();

    /// @brief Sets the parent for the specified task.
    void set_parent(const TaskId& task_id, const TaskId& parent);

    /// @brief Spawns the specified task and returns.
    /// @sa PrepareTask
    void spawn_task(const TaskId& task_id);

    /// @brief Spawns the specified task and doesn't return until it's completed.
    void spawn_task_and_wait(const TaskId& task_id);

    /// @brief Sets the maximum worker thread count
    /// @param num Number of threads, cannot be bigger than MAX_THREADS_COUNT
    void set_worker_count(uint32_t num);

    /// @brief Returns the number of worker threads running currently
    uint32_t worker_count() const;


protected:

    /// Class for handling individual tasks, use TaskManager::AllocTask to create tasks 
    struct Task
    {
        uint32_t id;
        uint32_t parent; // Index to parent node in task pool, Invalid<uint32_t> if no parent.

        // Number of open work items, this task isn't finished until this reaches zero
        volatile long num_work_items;

        WorkItem work_item;
    };

    /// @brief Class defining a worker thread used for executing tasks
    class WorkerThread : public Runnable
    {
    public:
        WorkerThread(TaskScheduler* owner) : _owner(owner) {}
        ~WorkerThread() {}

        /// @brief Starts the worker
        void start();

        // Runnable
        void run();

        /// @brief Is the thread running? 
        bool is_running() const { return _thread.is_running(); }

        /// @brief Returns the worker index in the task manager
        int get_worker_index();

    private:
        /// @brief Called when the thread starts idling, this call will be blocked until the owners wakeup semaphore is set.
        void idle();

        TaskScheduler* _owner;
        SimpleThread _thread;
    };

    //-------------------------------------------------------------------------------
    /// @brief Allocates a new tasks and assigns a new id to the specified TaskId object.
    Task* allocate_task(TaskId& task_id);
    /// @brief Releases a task
    void release_task(Task* task);
    //-------------------------------------------------------------------------------

    /// Tries to pop a task from the task queue
    bool pop_task(Task** task);

    /// Pushes a task to the task queue
    void push_task(Task* task);

    /// Runs the specified task
    void run_task(Task* task);

    /// Completes the specified task
    void complete_task(Task* task);

    //-------------------------------------------------------------------------------

    /// @brief Checks if the specified task has completed.
    bool has_completed(const TaskId& task_id);

    //-------------------------------------------------------------------------------

    /// @brief Wakes all worker threads 
    void wake_workers();

    /// @brief Waits for all workers to be ready
    void wait_for_workers();

    /// @brief Returns true if the manager is shutting down
    bool is_shutting_down() const;

    //-------------------------------------------------------------------------------

private:
    MemoryPool<Task, TASK_POOL_SIZE> _task_pool;

    std::vector<Task*> _task_queue;
    CriticalSection _task_lock;

    volatile long _next_task_id;

    std::vector<WorkerThread*> _workers;
    uint32_t _num_threads;
    uint32_t _num_processors; //<! Number of cores in this system

    Semaphore _sem_thread_sleep;
    Semaphore _sem_thread_wakeup;

    bool _shutting_down; //<! If the manager is shutting down

};

namespace scheduling
{
    void parallel_for(TaskScheduler* scheduler, ParallelForKernel fn, void* param, const Range& range);
};




#endif // __CORE_TASKSCHEDULER_H__
