// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Thread.h"

#include <process.h>

/// @file Thread_Win.cpp
/// @brief Threading implementations for Win32


//-------------------------------------------------------------------------------
ThreadLocalPtr::ThreadLocalPtr()
{
    _tls_index = TlsAlloc();
    assert(_tls_index != TLS_OUT_OF_INDEXES);
    set(NULL);
}
ThreadLocalPtr::~ThreadLocalPtr()
{
    TlsFree(_tls_index);
}
void ThreadLocalPtr::set(void* value)
{
    TlsSetValue(_tls_index, value);
}
void* ThreadLocalPtr::get() const
{
    return TlsGetValue(_tls_index);
}
//-------------------------------------------------------------------------------
SimpleThread::SimpleThread()
    : _thread_handle(NULL),
    _is_running(false),
    _runnable(NULL)
{
    _thread_payload.function = NULL;
    _thread_payload.params = NULL;
}
SimpleThread::~SimpleThread()
{
    if (_thread_handle)
    {
        CloseHandle(_thread_handle);
    }
}
//-------------------------------------------------------------------------------
bool SimpleThread::join(uint32_t time_out)
{
    if (_thread_handle != 0)
    {
        DWORD res = WaitForSingleObject((HANDLE)_thread_handle, ((time_out != 0) ? time_out : INFINITE));
        if (res != WAIT_OBJECT_0)
        {
            return false;
        }

        CloseHandle(_thread_handle);
        _thread_handle = NULL;
    }
    return true;
}
//-------------------------------------------------------------------------------
void SimpleThread::start_thread(unsigned(__stdcall *func)(void *), void* arg_list)
{
    _thread_handle = (void*)_beginthreadex(NULL, 0, func, arg_list, CREATE_SUSPENDED, &_thread_id);
    assert(_thread_handle);
    ResumeThread((HANDLE)_thread_handle);
    _is_running = true;
}

//-------------------------------------------------------------------------------
// Static methods

unsigned int SimpleThread::run_runnable(void* param)
{
    SimpleThread* thread = static_cast<SimpleThread*>(param);

    if (thread->_runnable)
        thread->_runnable->run();

    thread->_thread_handle = NULL;
    thread->_is_running = false;
    _endthreadex(0);
    return 0;
}
unsigned int SimpleThread::run_function(void* param)
{
    SimpleThread* thread = static_cast<SimpleThread*>(param);

    if (thread->_thread_payload.function)
        thread->_thread_payload.function(thread->_thread_payload.params);

    thread->_thread_handle = NULL;
    thread->_is_running = false;
    _endthreadex(0);
    return 0;
}

//-------------------------------------------------------------------------------

long thread::interlocked_increment(long volatile* addend)
{
    return ::InterlockedIncrement(addend);
}
long thread::interlocked_decrement(long volatile* addend)
{
    return ::InterlockedDecrement(addend);
}

long thread::interlocked_exchange_add(long volatile* addend, long value)
{
    return ::InterlockedExchangeAdd(addend, value);
}
unsigned long thread::interlocked_exchange_subtract(unsigned long volatile* addend, unsigned long value)
{
    return ::InterlockedExchangeSubtract(addend, value);
}
long thread::interlocked_compare_exchange(long volatile* dest, long exchange, long comparand)
{
    return (::InterlockedCompareExchange(dest, exchange, comparand) == comparand);
}
long thread::interlocked_exchange(long volatile* dest, long value)
{
    return ::InterlockedExchange(dest, value);
}

int64_t thread::interlocked_increment_64(int64_t volatile* addend)
{
    return ::InterlockedIncrement64(addend);
}
int64_t thread::interlocked_decrement_64(int64_t volatile* addend)
{
    return ::InterlockedDecrement64(addend);
}

int64_t thread::interlocked_exchange_add_64(int64_t volatile* addend, int64_t value)
{
    return ::InterlockedExchangeAdd64(addend, value);
}


//-------------------------------------------------------------------------------


