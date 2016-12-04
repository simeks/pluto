/// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNNDATION_PROXYALLOCATOR_H__
#define __FOUNNDATION_PROXYALLOCATOR_H__

#include "Memory.h"
#include "StlAllocator.h"

#include "Thread/Thread.h"



class BasicMemoryTracker;

//-------------------------------------------------------------------------------

/// @brief Proxy allocator used for debugging
///
/// An allocator working as a proxy for an actual allocator, for debugging
///	purposes it tracks all allocations going through it.
template<class MemoryTracker>
class ProxyAllocator : public Allocator
{
    Allocator&	_allocator;
    const char* _name;

    MemoryTracker _memory_tracker;

public:
    ProxyAllocator(const char* name, Allocator& backing);
    ~ProxyAllocator();

    void* allocate(size_t size);
    void* reallocate(void* p, size_t size);
    void free(void* p);

    size_t allocated_size(void* p);

    const char* name() const;

    MemoryTracker& tracker()  { return _memory_tracker; }
};

//-------------------------------------------------------------------------------

class NoTracking
{
public:
    NoTracking(ProxyAllocator<NoTracking>&) {}
    ~NoTracking() {}

    INLINE void on_allocate(void*, size_t) {}
    INLINE void on_free(void*) {}
};

#ifdef FLOW_MEMORY_TRACKING

class AllocationSizeLog
{
public:
    AllocationSizeLog() {}

    void push(void* p, size_t size)
    {
        _sizes[p] = size;
    }
    size_t pop(void* p)
    {
        size_t s = 0;
        auto it = _sizes.find(p);
        if (it != _sizes.end())
        {
            s = it->second;
            _sizes.erase(it);
        }
        return s;
    }
    size_t get(void* p)
    {
        size_t s = 0;
        auto it = _sizes.find(p);
        if (it != _sizes.end())
        {
            s = it->second;
        }
        return s;
    }

private:
    typedef std::unordered_map<void*, size_t, std::hash<void*>, std::equal_to<void*>, stl_debug_allocator<std::pair<void*, size_t>>> AllocationSizeMap;
    AllocationSizeMap _sizes;

};

class BasicMemoryTracker
{
    ProxyAllocator<BasicMemoryTracker>&	_allocator;

    volatile size_t _allocation_count;
    volatile size_t _bytes_allocated;

    AllocationSizeLog _size_log;

    const BasicMemoryTracker& operator=(const BasicMemoryTracker&) { return *this; }
public:
    BasicMemoryTracker(ProxyAllocator<BasicMemoryTracker>& owner);
    ~BasicMemoryTracker();

    void on_allocate(void* p, size_t s);
    void on_free(void* p);
};

class VerboseMemoryTracker
{
public:
    static const uint32_t MAX_STACKTRACE_DEPTH = 32;

    struct AllocationStackTrace
    {
        void* addresses[MAX_STACKTRACE_DEPTH];
    };

public:
    ProxyAllocator<VerboseMemoryTracker>& _allocator;

    // TODO: Change to alternative for std::hash_map
    typedef std::unordered_map<void*, AllocationStackTrace, std::hash<void*>, std::equal_to<void*>, stl_debug_allocator<std::pair<void*, AllocationStackTrace>>> StackTraceMap;
    StackTraceMap _allocation_stack_traces;
    CriticalSection _allocation_stack_traces_lock;
    static bool s_disable_stack_trace;

    volatile size_t _allocation_count;
    volatile size_t _bytes_allocated;

    AllocationSizeLog _size_log;

    const VerboseMemoryTracker& operator=(const VerboseMemoryTracker&) { return *this; }
public:
    VerboseMemoryTracker(ProxyAllocator<VerboseMemoryTracker>& owner);
    ~VerboseMemoryTracker();

    void on_allocate(void* p, size_t s);
    void on_free(void* p);

    /// @brief Prints memory stats about this proxy
    void print_memory_stats();
};

typedef ProxyAllocator<VerboseMemoryTracker> VerboseTraceAllocator;
typedef ProxyAllocator<BasicMemoryTracker> TraceAllocator;

#else 

typedef ProxyAllocator<NoTracking> TraceAllocator;
typedef ProxyAllocator<NoTracking> RemoteTraceAllocator;


#endif 

//-------------------------------------------------------------------------------

template<class MemoryTracker>
ProxyAllocator<MemoryTracker>::ProxyAllocator(const char* name, Allocator& backing)
    : _allocator(backing), _name(name), _memory_tracker(*this)
{
}

template<class MemoryTracker>
ProxyAllocator<MemoryTracker>::~ProxyAllocator()
{
}

template<class MemoryTracker>
void* ProxyAllocator<MemoryTracker>::allocate(size_t size)
{
    void* p = _allocator.allocate(size);
    _memory_tracker.on_allocate(p, size);
    return p;
}
template<class MemoryTracker>
void* ProxyAllocator<MemoryTracker>::reallocate(void* p, size_t size)
{
    _memory_tracker.on_free(p);
    p = _allocator.reallocate(p, size);
    _memory_tracker.on_allocate(p, size);
    return p;
}
template<class MemoryTracker>
void ProxyAllocator<MemoryTracker>::free(void* p)
{
    if (p == nullptr)
        return;

    _memory_tracker.on_free(p);
    _allocator.free(p);
}

template<class MemoryTracker>
const char* ProxyAllocator<MemoryTracker>::name() const
{
    return _name;
}


#endif // __FOUNNDATION_PROXYALLOCATOR_H__
