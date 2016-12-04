// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#ifdef FLOW_MEMORY_TRACKING

void* operator new(size_t size)
{
    return memory::default_allocator().allocate(size);
}

void* operator new[](size_t size)
{
    return memory::default_allocator().allocate(size);
}

void operator delete(void* p)
{
    memory::default_allocator().free(p);
}
void operator delete[](void* p)
{
    memory::default_allocator().free(p);
}
#endif
