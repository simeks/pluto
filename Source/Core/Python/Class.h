#ifndef __PYTHON_CLASS_H__
#define __PYTHON_CLASS_H__

#include "Object.h"

#include <typeindex>

namespace python
{
    typedef void(*ClassInit)(const python::Object& cls);

    /// Holder: Responsible for holding a value of some sort for a python object instance.
    ///
    /// Two available holders are for raw pointers (PtrHolder) and shared_ptr (SharedPtrHolder).
    /// PtrHolder : Holds a raw pointer, this is mainly used when we have a object instance created from
    ///             converting from a raw pointer. In this case we don't have a lot of information about
    ///             ownership and so on, therefore this will assume the user is responsible for not 
    class CORE_API Holder
    {
    public:
        Holder();
        virtual ~Holder();
    };

    template<typename T>
    class ValueHolder : public Holder
    {
    public:
        ValueHolder();

    private:
        T _v;
    };

    //template<typename T>
    //class PtrHolder : public Holder
    //{
    //public:
    //    PtrHolder();
    //    ~PtrHolder();

    //private:
    //    T* _p;
    //};

    //template<typename T>
    //class SharedPtrHolder : public Holder
    //{
    //public:
    //    SharedPtrHolder();
    //    ~SharedPtrHolder();

    //private:
    //    std::shared_ptr<T> _p;
    //};


    class CORE_API CppClassBase
    {
    public:
        /// Allocates a new holder, deallocate Holder with delete.
        virtual Holder* allocate() = 0;
    };

    template<typename T>
    class CppClass
    {
    public:
        Holder* allocate() OVERRIDE
        {
            return new ValueHolder<T>();
        }

    };

    /// @remark This function takes ownership of the cpp_class object, deleting it whenever done with it.
    CORE_API Object make_class(const char* name, CppClassBase* cpp_class);


    /// Helper function for creating a class
    /// @param init_class Function for initializing the class, e.g. setting methods, etc.
    template<typename T>
    Object make_class(const char* name);

    /// @brief Returns the python class object for the specified class
    template<typename T>
    const Object& class_object();

    template<typename T>
    Object make_instance(T* obj)
    {
        Object cls = class_object<T>();

    }

    /// Python -> Constructor wrapper
    template<typename T, typename ... TArgs>
    void class_init(T* self, TArgs... args);

    /// @brief Defines a __init__ method for the given class
    template<typename T, typename ... TArgs>
    void def_init(const Object& cls);


}

#include "Class.inl"

#endif // __PYTHON_CLASS_H__
