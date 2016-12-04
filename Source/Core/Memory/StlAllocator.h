// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_STLALLOCATOR_H__
#define __CORE_STLALLOCATOR_H__

/// @brief Custom allocator for STL
///
///	It's made so that we can use our memory::DebugAllocator with STL 
template<typename T>
class stl_debug_allocator : public std::allocator<T>
{
public:
    typedef T          value_type;
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;

    typedef T*         pointer;
    typedef const T*   const_pointer;

    typedef T&         reference;
    typedef const T&   const_reference;

    template<typename U>
    struct rebind
    {
        typedef stl_debug_allocator<U> other;
    };

    stl_debug_allocator() throw() {}
    stl_debug_allocator(const stl_debug_allocator&) throw() {}
    template<typename U> stl_debug_allocator(const stl_debug_allocator<U>&) throw() {}
    ~stl_debug_allocator() throw() {}

    pointer allocate(size_type n, const void* = 0)
    {
#ifdef FLOW_MEMORY_TRACKING
        return (pointer)memory::debug_allocator().allocate(n*sizeof(T));
#else
        return (pointer)memory::default_allocator().allocate(n*sizeof(T));
#endif
    }
    void deallocate(pointer p, size_type)
    {
#ifdef FLOW_MEMORY_TRACKING
        memory::debug_allocator().free(p);
#else
        memory::default_allocator().free(p);
#endif
    }

};

template <typename T, typename U>
inline bool operator==(const stl_debug_allocator<T>&, const stl_debug_allocator<U>){ return true; }

template <typename T, typename U>
inline bool operator!=(const stl_debug_allocator<T>&, const stl_debug_allocator<U>){ return false; }



#endif // __CORE_STLALLOCATOR_H__
