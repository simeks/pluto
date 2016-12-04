// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Event.h"

#include <process.h>


//-------------------------------------------------------------------------------
Event::Event(bool manual_reset, bool initial_state)
{
    _handle = (void*)CreateEvent(NULL, manual_reset, initial_state, NULL);
}
Event::~Event()
{
    CloseHandle(_handle);
}

void Event::set()
{
    SetEvent(_handle);
}

void Event::reset()
{
    ResetEvent(_handle);
}

void Event::wait()
{
    WaitForSingleObject(_handle, INFINITE);
}

bool Event::wait(uint32_t milliseconds)
{
    if (WaitForSingleObject(_handle, milliseconds) == WAIT_TIMEOUT)
        return false;
    return true;
}
//-------------------------------------------------------------------------------



