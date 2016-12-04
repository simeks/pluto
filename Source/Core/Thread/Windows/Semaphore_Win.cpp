// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Semaphore.h"

#include <process.h>



//-------------------------------------------------------------------------------
Semaphore::Semaphore(int n)
{
    // Max value = 0x7fffffff (MAXLONG)
    _handle = (void*)CreateSemaphore(NULL, n, 0x7fffffff, NULL);
}
Semaphore::~Semaphore()
{
    CloseHandle(_handle);
}

void Semaphore::set()
{
    ReleaseSemaphore(_handle, 1, NULL);
}

void Semaphore::wait()
{
    WaitForSingleObject(_handle, INFINITE);
}
//-------------------------------------------------------------------------------



