// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_MEMORY_H__
#define __CORE_MEMORY_H__

#include "Allocator.h"

//#define FLOW_MEMORY_TRACKING
//#define FLOW_VERBOSE_MEMORY_TRACKING

/// @brief Creates a new object of type T using the allocator a.
#define LAB_NEW(a, T, ...) (new ((a).allocate(sizeof(T))) T(__VA_ARGS__))

/// @brief Frees an object p of type T from allocator a.
#define LAB_DELETE(a, p) (memory::delete_obj(a, p))

/// @brief Creates a new array of type T with size N
#define LAB_NEW_ARRAY(a, T, N) (memory::new_array<T>(a, N))

/// @brief Frees an array p created by LAB_NEW_ARRAY
#define LAB_DELETE_ARRAY(a, p) (memory::delete_array(a, p))


/// @brief Memory namespace
namespace memory
{
    CORE_API void initialize();
    CORE_API void shutdown();

    Allocator& default_allocator();
#ifdef FLOW_MEMORY_TRACKING
    Allocator& debug_allocator();
#endif

    /// @brief Allocated the specified amount of memory
    void* malloc(size_t size);

    /// @brief Changes the size of the specified memory block
    void* realloc(void* p, size_t size);

    /// @brief Frees memory previously allocated with Malloc()
    void free(void* p);

    /// @brief Helper function for deleting an object
    template<typename T> void delete_obj(Allocator& allocator, T* p);

    /// @brief Helper function for allocating arrays with a specified allocator
    template<typename T> T* new_array(Allocator& allocator, size_t N);

    /// @brief Helper function for freeing arrays with a specified allocator
    template<typename T> void delete_array(Allocator& allocator, T* p);

    /// @brief Returns the result of advancing pointer p by given number of bytes 
    INLINE void* pointer_add(void *p, size_t bytes);
    /// @brief Returns the result of advancing pointer p by given number of bytes 
    INLINE const void* pointer_add(const void *p, size_t bytes);

    /// @brief Returns the result of moving pointer p back by given number of bytes 
    INLINE void* pointer_sub(void *p, size_t bytes);
    /// @brief Returns the result of advancing pointer p back by given number of bytes 
    INLINE const void* pointer_sub(const void *p, size_t bytes);

    /// @brief Aligns the pointer p to the specified alignment
    INLINE void* align_forward(void* p, size_t alignment);

}

template<typename T> void memory::delete_obj(Allocator& allocator, T* p)
{
    if (p)
    {
        destruct<T>(p);
        allocator.free(p);
    }
}
template<typename T> T* memory::new_array(Allocator& allocator, size_t n)
{
    union
    {
        void*	_p;
        size_t*	_n;
        T*		_obj;
    };

    // Since we need to worry about destruction of the elements we store the number
    //	of elements immediately before our array.
    _p = allocator.allocate(sizeof(T)* n + sizeof(size_t));
    *_n++ = n;

    for (size_t i = 0; i < n; ++i)
    {
        // Construct our instances with placement new
        new(_obj)T;
        ++_obj;
    }

    return _obj - n;
}
template<typename T> void memory::delete_array(Allocator& allocator, T* p)
{
    if (p)
    {
        // Retrieve our N that we stored immediately before our array
        size_t* pn = reinterpret_cast<size_t*>(p)-1;
        size_t n = *pn;

        // Call the destructor for all elements
        for (size_t i = 0; i < n; ++i)
        {
            p[i].~T();
        }
        // Free all memory
        allocator.free(pn);
    }
}
void* memory::pointer_add(void *p, size_t bytes)
{
    return (void*)((char *)p + bytes);
}
const void* memory::pointer_add(const void *p, size_t bytes)
{
    return (const void*)((const char *)p + bytes);
}
void* memory::pointer_sub(void *p, size_t bytes)
{
    return (void*)((char *)p - bytes);
}
const void* memory::pointer_sub(const void *p, size_t bytes)
{
    return (const void*)((const char *)p - bytes);
}
void* memory::align_forward(void* p, size_t alignment)
{
    uintptr_t pi = uintptr_t(p);
    size_t mod = pi % alignment;
    if (mod)
        pi += (alignment - mod);
    return (void*)pi;
}


#endif // __CORE_MEMORY_H__

