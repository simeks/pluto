// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_LOCK_H__
#define __THREAD_LOCK_H__

#include <Core/Platform/WindowsWrapper.h>

/// @brief Win32 specific mutex
class CORE_API Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
    bool try_lock();

private:
#ifdef PLUTO_PLATFORM_WINDOWS
    void* _handle;
#else
    pthread_mutex_t _mutex;
#endif
};


#ifdef PLUTO_PLATFORM_WINDOWS

/// @brief Critical section lock for win32
class CORE_API CriticalSection
{
public:
    CriticalSection();
    ~CriticalSection();

    void lock();
    void unlock();
    bool try_lock();

private:
    CRITICAL_SECTION _cs;
};

#else
typedef Mutex CriticalSection;

#endif


/// @brief Scoped lock to simplify synchronization in a scope 
template<typename T>
class ScopedLock
{
public:
    ScopedLock(T& lock) : _lock(lock)
    {
        _lock.lock();
    }
    ~ScopedLock()
    {
        _lock.unlock();
    }
private:
    T& _lock;

    ScopedLock();
    ScopedLock(const ScopedLock&);
    ScopedLock& operator=(const ScopedLock&);
};




#endif // __THREAD_LOCK_H__
