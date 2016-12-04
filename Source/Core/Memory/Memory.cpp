// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Memory.h"
#include "ProxyAllocator.h"

class HeapAllocator : public Allocator
{
public:
    /// @brief Allocated the specified amount of memory with the specified alignment
    void* allocate(size_t size)
    {
        return ::malloc(size);
    }
    void* reallocate(void* p, size_t size)
    {
        return ::realloc(p, size);
    }
    void free(void* p)
    {
        ::free(p);
    }
};

namespace memory
{
    static const int ALLOCATOR_SIZE = sizeof(HeapAllocator);
    char g_allocator_buffer[2 * ALLOCATOR_SIZE];

    HeapAllocator* g_default_alloc = nullptr;
#ifdef FLOW_MEMORY_TRACKING
    HeapAllocator* g_debug_alloc = nullptr;
#endif

#ifdef FLOW_VERBOSE_MEMORY_TRACKING
    VerboseTraceAllocator* g_default_proxy_allocator = nullptr;
#else
    TraceAllocator* g_default_proxy_allocator = nullptr;
#endif
};

void memory::initialize()
{
    g_default_alloc = new (g_allocator_buffer)HeapAllocator();

#ifdef FLOW_MEMORY_TRACKING
    g_debug_alloc = new (g_allocator_buffer + ALLOCATOR_SIZE) HeapAllocator();
#ifdef FLOW_VERBOSE_MEMORY_TRACKING
    g_default_proxy_allocator = LAB_NEW(*g_debug_alloc, VerboseTraceAllocator, "Default", *g_default_alloc);
#else
    g_default_proxy_allocator = LAB_NEW(*g_debug_alloc, TraceAllocator, "Default", *g_default_alloc);
#endif
#endif
}
void memory::shutdown()
{
#ifdef FLOW_MEMORY_TRACKING
    if (g_default_proxy_allocator)
    {
        LAB_DELETE(*g_debug_alloc, g_default_proxy_allocator);
    }
#endif
    // Because the allocator is stored on the stack and we can't use delete we have 
    //	to call the destructor ourself
    g_default_alloc->~HeapAllocator();
    g_default_alloc = nullptr;

#ifdef FLOW_MEMORY_TRACKING
    g_debug_alloc->~HeapAllocator();
    g_debug_alloc = nullptr;
#endif
}
Allocator& memory::default_allocator()
{
#ifdef FLOW_MEMORY_TRACKING
    return *g_default_proxy_allocator;
#else
    return *g_default_alloc;
#endif
}
#ifdef FLOW_MEMORY_TRACKING
Allocator& memory::debug_allocator()
{
    return *g_debug_alloc;
}
#endif

void* memory::malloc(size_t size)
{
    assert(g_default_alloc);
    return g_default_alloc->allocate(size);
}
void* memory::realloc(void* p, size_t size)
{
    assert(g_default_alloc);
    return g_default_alloc->reallocate(p, size);
}
void memory::free(void* p)
{
    assert(g_default_alloc);
    g_default_alloc->free(p);
}

