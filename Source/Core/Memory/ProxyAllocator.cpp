// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ProxyAllocator.h"
#include "Platform/System.h"

#ifdef FLOW_MEMORY_TRACKING
//-------------------------------------------------------------------------------
BasicMemoryTracker::BasicMemoryTracker(ProxyAllocator<BasicMemoryTracker>& owner)
    : _allocator(owner)
{
    _allocation_count = 0;
    _bytes_allocated = 0;
}
BasicMemoryTracker::~BasicMemoryTracker()
{
    if (_allocation_count != 0 || _bytes_allocated != 0)
    {
        logging::warning("Memory leak detected! (Allocator: %s, Allocations: %u, Bytes: %u)",
            _allocator.name(), _allocation_count, _bytes_allocated);
    }

    assert_msg((_allocation_count == 0 || _bytes_allocated == 0), "Memory leak detected!");
}
void BasicMemoryTracker::on_allocate(void* p, size_t s)
{
    _size_log.push(p, s);
    if (s == 0)
        return;	// Nothing allocated, most likely using an allocator not tracking size


#ifdef FLOW_PLATFORM_WIN64
    thread::interlocked_increment_64((int64_t*)&_allocation_count);
    thread::interlocked_exchange_add_64((int64_t*)&_bytes_allocated, s);
#else
    thread::interlocked_increment((long*)&_allocation_count);
    thread::interlocked_exchange_add((long*)&_bytes_allocated, s);
#endif
}
void BasicMemoryTracker::on_free(void* p)
{
    if (!p) // Ignore NULL pointers
        return;

    size_t bytes = _size_log.pop(p);
    if (bytes == 0)
        return;	// Nothing allocated, most likely using an allocator not tracking size
#ifdef FLOW_PLATFORM_WIN64
    thread::interlocked_decrement_64((int64_t*)&_allocation_count);
    thread::interlocked_exchange_add_64((int64_t*)&_bytes_allocated, -(int64_t)bytes);
#else
    thread::interlocked_decrement((long*)&_allocation_count);
    thread::interlocked_exchange_subtract((unsigned long*)&_bytes_allocated, bytes);
#endif
}
//-------------------------------------------------------------------------------

bool VerboseMemoryTracker::s_disable_stack_trace = false;

VerboseMemoryTracker::VerboseMemoryTracker(ProxyAllocator<VerboseMemoryTracker>& owner)
    : _allocator(owner)
{
    _allocation_count = 0;
    _bytes_allocated = 0;
}
VerboseMemoryTracker::~VerboseMemoryTracker()
{
    if (_allocation_count != 0 || _bytes_allocated != 0)
    {
        logging::warning("Memory leak detected!");
        print_memory_stats();
    }

    assert_msg((_allocation_count == 0 || _bytes_allocated == 0), "Memory leak detected!");
}
void VerboseMemoryTracker::on_allocate(void* p, size_t s)
{
    _size_log.push(p, s);
    if (s == 0)
        return;	// Nothing allocated, most likely using an allocator not tracking size


#ifdef FLOW_PLATFORM_WIN64
    thread::interlocked_increment_64((int64_t*)&_allocation_count);
    thread::interlocked_exchange_add_64((int64_t*)&_bytes_allocated, s);
#else
    thread::interlocked_increment((long*)&_allocation_count);
    thread::interlocked_exchange_add((long*)&_bytes_allocated, s);
#endif

    // We have to mark the stack trace as disabled when doing a stack trace ourself,
    //	because there's a chance for recursion if (in this case) std::map decides to
    //	allocate more memory when we're storing our stack trace. This would otherwise
    //	cause deadlocks.
    if (!s_disable_stack_trace)
    {
        ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
        s_disable_stack_trace = true;

        AllocationStackTrace stack_trace;
        memset(&stack_trace, 0, sizeof(AllocationStackTrace));
        system_util::stack_trace(stack_trace.addresses, MAX_STACKTRACE_DEPTH, 1);
        _allocation_stack_traces[p] = stack_trace;

        s_disable_stack_trace = false;
    }
}
void VerboseMemoryTracker::on_free(void* p)
{
    if (!p) // Ignore NULL pointers
        return;

    size_t bytes = _size_log.pop(p);
    if (bytes == 0)
        return;	// Nothing allocated, most likely using an allocator not tracking size

#ifdef FLOW_PLATFORM_WIN64
    thread::interlocked_decrement_64((int64_t*)&_allocation_count);
    thread::interlocked_exchange_add_64((int64_t*)&_bytes_allocated, -(int64_t)bytes);
#else
    thread::interlocked_decrement((long*)&_allocation_count);
    thread::interlocked_exchange_subtract((unsigned long*)&_bytes_allocated, bytes);
#endif

    if (!s_disable_stack_trace)
    {
        ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
        s_disable_stack_trace = true;

        if (_allocation_stack_traces.find(p) != _allocation_stack_traces.end())
            _allocation_stack_traces.erase(p);

        s_disable_stack_trace = false;
    }
}
void VerboseMemoryTracker::print_memory_stats()
{
    const char* name = _allocator.name();
    if (!name)
        name = "Unnamed";

    logging::info("Allocator: %s, Allocations: %d, Bytes: %d", name, _allocation_count, _bytes_allocated);
    logging::info("Unfreed allocations:");

    if (!s_disable_stack_trace)
    {
        ScopedLock<CriticalSection> scoped_lock(_allocation_stack_traces_lock);
        s_disable_stack_trace = true;

        std::string symbol;

        StackTraceMap::iterator it, end;
        end = _allocation_stack_traces.end();
        for (it = _allocation_stack_traces.begin(); it != end; ++it)
        {
            logging::info("Allocation: 0x%p (Size: %u):", it->first, _size_log.get(it->first));
            for (uint32_t i = 0; i < MAX_STACKTRACE_DEPTH; ++i)
            {
                void* addr = it->second.addresses[i];
                if (addr != nullptr)
                {
                    symbol.clear();
                    system_util::address_symbol(symbol, addr);
                    logging::info("\t%s", symbol.c_str());
                }
            }
        }

        s_disable_stack_trace = false;
    }
}



#endif 

