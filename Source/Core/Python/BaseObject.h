#ifndef __PYTHON_BASE_OBJECT_H__
#define __PYTHON_BASE_OBJECT_H__

#include <Core/Python/Class.h>
#include <Core/Python/Object.h>

/*
BaseObject:
This is an alternative to the typical way of exposing a class where the PyObject and the class instance
    is very loosely based. For example, converting a class object to Python will result in a new PyObject
    with a borrowed pointer to the object being created.
BaseObject will take care of its own PyObject, only creating a single one. Converting between C++ and Python
    will perform no unnecessary object allocations and the Python GC will be the one responsible for the
    objects life-time. In C++ the user can explicitly manage the reference count.
*/

namespace python
{
    /// Wrapper around PyObject* that lets users expose C++ classes to Python 
    ///     while inheriting some Python functionality such as GC.
    /// See addref() and release() for reference management
    /// Objects derived from BaseObject for the purpose of use with Python should
    ///     only be allocated using make_object(). Also, user should never use 
    ///     delete directly on objects derived from BaseObject. Python GC is responsible
    ///     for deleting this object.
    class CORE_API BaseObject
    {
    public:
        BaseObject();
        virtual ~BaseObject();

        /// Increases the reference count of the object
        void addref();
        /// Decreases the reference count of the object
        void release();

        python::Object attr(const char* name);

        /// Returns the wrapped PyObject*
        PyObject* ptr();

    protected:
        void initialize_python(PyObject* obj);

        template<typename T, typename ... TArgs>
        friend T* make_object(TArgs... args);
        
        friend CORE_API void initialize_object(PyObject*, BaseObject*);

    private:
        PyObject* _obj;
        PyTypeObject* _type;

    };

    template<typename T>
    class Ref
    {
    public:
        Ref(T* ptr);
        ~Ref();

        T* operator->() const;

        template<typename TOther>
        Ref(const Ref<TOther>& other);
        
        template<typename TOther>
        Ref<T>& operator=(const Ref<TOther>& other);

    private:
        T* _ptr;
    };

    template<typename TClass, typename ... TArgs>
    TClass* make_object(TArgs... args);

}

#include "BaseObject.inl"

#endif // __PYTHON_BASE_OBJECT_H__
