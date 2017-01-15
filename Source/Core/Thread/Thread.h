// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_H__
#define __THREAD_H__

#include "Platform/WindowsWrapper.h"

#include "Lock.h"
#include "Semaphore.h"
#include "Event.h"


/// @file Thread.h
/// @brief Threading utilities


/// @brief Thread local storage
class ThreadLocalPtr
{
public:
    ThreadLocalPtr();
    ~ThreadLocalPtr();

    void set(void* value);
    void* get() const;

private:
#ifdef PLUTO_PLATFORM_WINDOWS
    DWORD _tls_index;
#else
    pthread_key_t _key;
#endif
};



/// @brief Thread runnable 
class Runnable
{
public:
    virtual ~Runnable() {}
    virtual void run() = 0;
};

class CORE_API SimpleThread
{
public:
    typedef void(*ThreadFunction)(void*);

    SimpleThread();
    virtual ~SimpleThread();


    /// @brief Starting the thread with a runnable object
    virtual void start(Runnable* runnable);

    /// @brief Starting the thread with a function and parameters
    virtual void start(ThreadFunction thread_function, void* params = NULL);

    /// Waits for this thread to finish execution
    /// @param time_out Maximum time (In milliseconds) to wait, waits indefinitely if set to zero.
    /// @return True if the thread finished.
    bool join(uint32_t time_out = 0);

    /// @brief Is the thread running?
    bool is_running() const { return _is_running; }

protected:
    struct ThreadPayload
    {
        ThreadFunction function;
        void* params;
    };

#ifdef PLUTO_PLATFORM_WINDOWS
    HANDLE _thread_handle;
    uint32_t _thread_id;

#else
    pthread_t _thread_handle;
#endif

    bool _is_running;

    ThreadPayload _thread_payload;
    Runnable* _runnable;

private:
    SimpleThread(const SimpleThread&);
    void operator=(const SimpleThread&);


#ifdef PLUTO_PLATFORM_WINDOWS
    void start_thread(unsigned(__stdcall *func)(void *), void* arg_list);

    static unsigned int __stdcall run_runnable(void* param);
    static unsigned int __stdcall run_function(void* param);

#else
    void start_thread(void* (*func)(void *), void* arg_list);

    static void* run_runnable(void* param);
    static void* run_function(void* param);
#endif
};



namespace thread
{
    long interlocked_increment(long volatile* addend);
    long interlocked_decrement(long volatile* addend);

    /// @return The initial value of the addend parameter.
    long interlocked_exchange_add(long volatile* addend, long value);
    unsigned long interlocked_exchange_subtract(unsigned long volatile* addend, unsigned long value);

    /// @return The initial value of the dest parameter.
    long interlocked_compare_exchange(long volatile* dest, long exchange, long comparand);

    /// @return Initial value of dest.
    long interlocked_exchange(long volatile* dest, long value);

    int64_t interlocked_increment_64(int64_t volatile* addend);
    int64_t interlocked_decrement_64(int64_t volatile* addend);

    int64_t interlocked_exchange_add_64(int64_t volatile* addend, int64_t value);

}; // namespace thread




#endif // __THREAD_H__
