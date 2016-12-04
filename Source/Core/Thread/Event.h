// Copyright 2008-2014 Simon Ekström

#ifndef __THREAD_EVENT_H__
#define __THREAD_EVENT_H__


/// @brief Synchronization event
class Event
{
public:
    /// @brief Constructor
    /// @param manual_reset Specifies if this event needs a manual reset of if it should 
    ///                     be automatically reset after releasing a signaled thread.
    /// @param initial_value Specified initial state for the event, true for signaled state, false for unsignaled.
    Event(bool manual_reset = false, bool initial_value = false);
    ~Event();

    /// @brief Set the event to a signaled state
    void set();
    /// @brief Reset the event to an unsignaled state
    void reset();

    /// @brief Wait indefinitely for the event to become signaled
    void wait();
    /// @brief Wait with a time limit for the event to become signaled
    ///
    ///	@return False if the wait timed out.
    bool wait(uint32_t milliseconds);

private:
#ifdef PLUTO_PLATFORM_WINDOWS
    void* _handle;
#else
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    volatile bool _state;
    bool _auto_reset;
#endif

};





#endif // __THREAD_EVENT_H__
