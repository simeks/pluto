#ifndef __PYTHON_CLASS_H__
#define __PYTHON_CLASS_H__

#include "Object.h"

#include <typeindex>

namespace python
{
    typedef void(*ClassInit)(const python::Object& cls);

    /// Holder: Responsible for holding a value of some sort for a python object instance. 
    class CORE_API Holder
    {
    public:
        Holder();
        virtual ~Holder();

        virtual void* ptr() = 0;
    };

    template<typename T>
    class PtrHolder : public Holder
    {
    public:
        /// Default constructor:
        /// Allocates memory for the given type but performs no initialization.
        /// The allocated memory will be deleted whenever the holder is destroyed
        PtrHolder();

        /// Constructor
        /// This constructor takes an already existing pointer, here we take no ownership
        /// of the pointer so it is still the users responsibility. Be careful, this of course
        /// requires the pointer to be valid as long as the python instance is available.
        /// The holder will not free the memory pointed to when destroyed.
        PtrHolder(T* ptr);

        ~PtrHolder();
        void* ptr();

    private:
        T* _p;
        bool _own; // Do we own the pointer?
    };

    class CORE_API CppClassBase
    {
    public:
        /// Allocates a new holder, deallocate Holder with delete.
        virtual Holder* allocate() = 0;
    };

    template<typename T>
    class CppClass : public CppClassBase
    {
    public:
        Holder* allocate() OVERRIDE;
    };

    /// @remark This function takes ownership of the cpp_class object, deleting it whenever done with it.
    CORE_API Object make_class(const char* name, CppClassBase* cpp_class);

    /// Creates a new instance of the specified type
    /// @param holder Value holder, this will be deleted whenever GC collects the created instance.
    /// @return The new instance or None if failed
    CORE_API Object make_instance(PyTypeObject* type, Holder* holder);

    /// Returns the value holder for the given instance.
    CORE_API Holder* holder(PyObject* instance);

    /// Helper function for creating a class
    /// @param init_class Function for initializing the class, e.g. setting methods, etc.
    template<typename TClass>
    Object make_class(const char* name);

    template<typename TClass>
    Object make_instance(TClass* value);

    /// Python -> Constructor wrapper
    template<typename TClass, typename ... TArgs>
    void class_init(TClass* self, TArgs... args);

    /// @brief Defines a __init__ method for the given class
    template<typename TClass, typename ... TArgs>
    void def_init(const Object& cls);

    /// @brief Defines a class method
    template<typename TClass, typename TReturn, typename ... TArgs>
    void def(const Object& cls, const char* name, 
        TReturn(TClass::*meth)(TArgs...), const char* doc = nullptr);

    /// @brief Defines a varargs class method
    template<typename TClass, typename TReturn, typename ... TArgs>
    void def_varargs(const Object& cls, const char* name, 
        TReturn(TClass::*meth)(const Tuple&), const char* doc = nullptr);

    /// @brief Defines a varargs/keywords class method
    template<typename TClass, typename TReturn, typename ... TArgs>
    void def_varargs_keywords(const Object& cls, const char* name,
        TReturn(TClass::*meth)(const Tuple&, const Dict&), const char* doc = nullptr);
}

#include "Class.inl"

#endif // __PYTHON_CLASS_H__
