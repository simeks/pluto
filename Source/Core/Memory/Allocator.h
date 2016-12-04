// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_ALLOCATOR_H__
#define __CORE_ALLOCATOR_H__

#include <Core/NonCopyable.h>

/// @brief Interface for allocators
class Allocator : public NonCopyable
{
public:
    virtual ~Allocator() {}

    /// @brief Allocated the specified amount of memory with the specified alignment
    virtual void* allocate(size_t size) = 0;

    /// @brief Changes the size of the specified memory block
    virtual void* reallocate(void* p, size_t size) = 0;

    /// @brief Frees memory previously allocated with Allocate()
    virtual void free(void* p) = 0;



};


#endif // __CORE_ALLOCATOR_H__

