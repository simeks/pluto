// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"
#include "Thread/Lock.h"

#include "../System.h"

///
/// @file System_Win.cpp
///	@brief Some platform dependent functions for the system
///

#pragma warning(push)
#pragma warning(disable: 4091) // dbghelp.h : warning C4091: 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)

#pragma comment ( lib, "dbghelp.lib" )


#if PLUTO_PLATFORM_WIN64
#define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_AMD64
#else
#define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_I386
#endif


namespace system_util
{
    CriticalSection g_stack_walk_lock;

    void sym_initialize();
};
void system_util::sym_initialize()
{
    static bool is_sym_initialized = false;
    if (!is_sym_initialized)
    {
        BOOL res = ::SymInitialize(GetCurrentProcess(), NULL, true);
        if (res)
        {
            //console::print("Symbol handler initialized\n");
        }
        else
        {
            //console::error("Failed to initialize symbol handler (Error code: %u)", ::GetLastError());
        }

        is_sym_initialized = true;
    }
}

uint32_t system_util::stack_trace(void** addresses, uint32_t max_addresses, uint32_t skip_count)
{
#ifdef PLUTO_PLATFORM_WIN64
    //assert(false); // Not implemtented
#endif

    assert(addresses);
    assert(max_addresses != 0);

    ScopedLock<CriticalSection> scoped_lock(g_stack_walk_lock);
    sym_initialize();

    HANDLE process_handle = GetCurrentProcess();
    HANDLE thread_handle = GetCurrentThread();

    // Capture current context
    CONTEXT context;
    RtlCaptureContext(&context);

    // Intialize our stack frame struct
    STACKFRAME64 frame;
    ZeroMemory(&frame, sizeof(STACKFRAME64));

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

#ifdef PLUTO_PLATFORM_WIN64
    frame.AddrPC.Offset = context.Rip;
    frame.AddrFrame.Offset = context.Rbp;
    frame.AddrStack.Offset = context.Rsp;
#else
    frame.AddrPC.Offset = context.Eip;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrStack.Offset = context.Esp;
#endif

    // Skip the call here
    skip_count++;
    // Just walk through and skip storing our first skipped frames
    for (uint32_t i = 0; i < skip_count; ++i)
    {
        BOOL res = StackWalk64(IMAGE_FILE_ARCH, process_handle, thread_handle, &frame, &context, NULL,
            SymFunctionTableAccess64, SymGetModuleBase64, NULL);
        if (!res || frame.AddrPC.Offset == 0) // Is this the end of our walk?
        {
            return 0;
        }
    }

    for (uint32_t i = 0; i < max_addresses; ++i)
    {
        BOOL res = StackWalk64(IMAGE_FILE_ARCH, process_handle, thread_handle, &frame, &context, NULL,
            SymFunctionTableAccess64, SymGetModuleBase64, NULL);

        if (!res || frame.AddrPC.Offset == 0) // Is this the end of our walk?
        {
            return i;
        }
        addresses[i] = (void*)frame.AddrPC.Offset;
    }
    return max_addresses;
}
void system_util::address_symbol(std::string& symbol, void* address)
{
    assert(address);
    ScopedLock<CriticalSection> scoped_lock(g_stack_walk_lock);
    sym_initialize();

    HANDLE process_handle = GetCurrentProcess();

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)buffer;

    symbol_info->MaxNameLen = MAX_SYM_NAME;
    symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
    if (SymFromAddr(process_handle, (DWORD64)address, NULL, symbol_info))
    {
        symbol_info->Name[MAX_SYM_NAME - 1] = '\0';
        symbol += symbol_info->Name; symbol += " ";
    }
    else
    {
        symbol += "(Undefined) ";
    }

    DWORD displacement = 0;
    IMAGEHLP_LINE64 line_info;
    if (SymGetLineFromAddr64(process_handle, (DWORD64)address, &displacement, &line_info))
    {
        char line_number[8];
        sprintf(line_number, "%u", line_info.LineNumber);
        line_number[7] = '\0';
        symbol += "(";
        symbol += line_info.FileName;
        symbol += ":";
        symbol += line_number;
        symbol += ")";
    }
    else
    {
        symbol += "(Undefined)";
    }
}


//-------------------------------------------------------------------------------
void system_util::system_info(SystemInfo& info)
{
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);

    info.num_processors = si.dwNumberOfProcessors;
    info.page_size = si.dwPageSize;
}
//-------------------------------------------------------------------------------


