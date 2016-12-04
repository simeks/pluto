// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Lock.h"

#include <process.h>


//-------------------------------------------------------------------------------

Mutex::Mutex()
{
    _handle = CreateMutex(NULL, FALSE, NULL);
}
Mutex::~Mutex()
{
    CloseHandle(_handle);
}

void Mutex::lock()
{
    WaitForSingleObject(_handle, INFINITE);
}

void Mutex::unlock()
{
    ReleaseMutex(_handle);
}

bool Mutex::try_lock()
{
    return WaitForSingleObject(_handle, 0) != WAIT_TIMEOUT;
}

//-------------------------------------------------------------------------------
CriticalSection::CriticalSection()
{
    InitializeCriticalSection(&_cs);
}
CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&_cs);
}

void CriticalSection::lock()
{
    EnterCriticalSection(&_cs);
}

void CriticalSection::unlock()
{
    LeaveCriticalSection(&_cs);
}

bool CriticalSection::try_lock()
{
    return TryEnterCriticalSection(&_cs) != FALSE;
}

//-------------------------------------------------------------------------------

