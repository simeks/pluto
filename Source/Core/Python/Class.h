#ifndef __PYTHON_CLASS_H__
#define __PYTHON_CLASS_H__

#include "Function.h"
#include "Object.h"

#include <memory>

namespace python
{
    class Holder;
    class Module;

    typedef struct {
        PyObject_HEAD;
        PyObject* dict;
        Holder* holder;
    } Instance;

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
        /// @param delete_ptr Specifies whether to delete the ptr when the holder is destroyed or not.
        PtrHolder(T* ptr, bool delete_ptr = false);

        ~PtrHolder();
        void* ptr();

    private:
        T* _p;
        bool _own; // Do we own the pointer?
    };

    template<typename T>
    class UniquePtrHolder : public Holder
    {
    public:
        UniquePtrHolder();
        UniquePtrHolder(std::unique_ptr<T> ptr);
        ~UniquePtrHolder();

        void* ptr();

    private:
        std::unique_ptr<T> _p;
    };

    /// PyObject wrapper for class objects
    class CORE_API Class : public Object
    {
    public:
        Class();
        Class(PyObject* obj);
        /// Constructor for borrowed references, this will increase the ref count of obj
        Class(Borrowed obj);

        /// @brief Defines a __init__ method for the given class
        template<typename TClass, typename ... TArgs>
        void def_init();

        /// @brief Defines a __init__ method for the given class
        /// Requires a constructor: TClass(const Tuple&)
        template<typename TClass>
        void def_init_varargs();

        /// @brief Defines a __init__ method for the given class
        /// Requires a constructor: TClass(const Tuple&, const Dict&)
        template<typename TClass>
        void def_init_varargs_keywords();

        /// @brief Defines a class method
        template<typename TClass, typename TReturn, typename ... TArgs>
        void def(const char* name, TReturn(TClass::*meth)(TArgs...), 
            const char* doc = nullptr);

        /// @brief Defines a class method
        template<typename TClass, typename TReturn, typename ... TArgs>
        void def(const char* name, TReturn(TClass::*meth)(TArgs...) const,
            const char* doc = nullptr);

        /// @brief Defines a varargs class method
        template<typename TClass, typename TReturn>
        void def_varargs(const char* name, TReturn(TClass::*meth)(const Tuple&), 
            const char* doc = nullptr);

        /// @brief Defines a varargs class method
        template<typename TClass, typename TReturn>
        void def_varargs(const char* name, TReturn(TClass::*meth)(const Tuple&) const,
            const char* doc = nullptr);

        /// @brief Defines a varargs/keywords class method
        template<typename TClass, typename TReturn>
        void def_varargs_keywords(const char* name, TReturn(TClass::*meth)(const Tuple&, const Dict&), 
            const char* doc = nullptr);

        /// @brief Defines a varargs/keywords class method
        template<typename TClass, typename TReturn>
        void def_varargs_keywords(const char* name, TReturn(TClass::*meth)(const Tuple&, const Dict&) const,
            const char* doc = nullptr);

        /// @brief Defins a static member
        template<typename T>
        void def(const char* name, const T& obj);
    };

    /// @remark This function takes ownership of the cpp_class object, deleting it whenever done with it.
    /// @param base_type Specifies the base type of the object, currently only one base type is allowed. 
    ///                  Will inherit the base instance type if nullptr.
    CORE_API Class make_class(const char* name, PyTypeObject* base_type = nullptr, const char* doc = nullptr);

    /// Creates a new instance of the specified type
    /// @param holder Value holder, this will be deleted whenever GC collects the created instance.
    /// @return The new instance or None if failed
    CORE_API Object make_instance(PyTypeObject* type, Holder* holder);

    /// Sets the module for the specified class
    CORE_API void set_module(const Class& cls, const Module& module);

    /// Helper function for creating a class
    template<typename TClass, typename TBaseClass = TClass>
    Class make_class(const char* name, const char* doc = nullptr);

    template<typename TClass>
    Object make_instance(TClass* value);

    /// Python -> Constructor wrapper
    template<typename TClass, typename ... TArgs>
    void class_init(PyObject* self, TArgs... args);

}

#include "Class.inl"

#endif // __PYTHON_CLASS_H__
