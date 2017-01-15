// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_SEMAPHORE_H__
#define __THREAD_SEMAPHORE_H__


/// @brief Semaphore
class CORE_API Semaphore
{
public:
    /// @brief Constructor
    ///
    /// @param n Start value, must be greater than zero.
    Semaphore(int n = 0);
    ~Semaphore();

    /// @brief Increase semaphores value by one
    void set();

    /// @brief Wait indefinitely for the semaphore to become signalled
    ///
    /// Waits for semaphore to become signalled, than decrements the semaphores value by one.
    void wait();



private:
#ifdef PLUTO_PLATFORM_WINDOWS
    void*	_handle;
#else
    int _n;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
#endif
};




#endif // __THREAD_SEMAPHORE_H__
