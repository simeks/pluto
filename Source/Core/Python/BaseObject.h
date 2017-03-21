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

#define PYTHON_OBJECT(TClass, TSuperClass) \
    public: \
        typedef TSuperClass Super; \
        typedef TClass ThisClass; \
    \
        static python::Class static_class(); \
    \
        virtual python::BaseObject* clone() const; \
    private: \
        static void init_python_type(python::Class cls);


#define PYTHON_OBJECT_IMPL_DOC(TClass, Name, Doc) \
    python::Class TClass::static_class() \
    { \
        static python::Class _class; \
        if (_class.is_none()) \
        { \
            if (!std::is_same<TClass, TClass::Super>::value) \
                TClass::Super::static_class(); /* Ensure super class is initialized */ \
            _class = python::make_class<TClass, TClass::Super>(#Name, #Doc); \
            init_python_type(_class); \
        } \
        return _class; \
    } \
    python::BaseObject* TClass::clone() const \
    { \
        return new TClass(*this); \
    } \
    void TClass::init_python_type(python::Class cls)
    
#define PYTHON_OBJECT_IMPL(TClass, Name) PYTHON_OBJECT_IMPL_DOC(TClass, Name, "")


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
        PYTHON_OBJECT(BaseObject, BaseObject);
    public:
        BaseObject();
        virtual ~BaseObject();

        bool has_attribute(const char* name) const;
        
        Object attribute(const char* name) const;
        void set_attribute(const char* name, const Object& obj);

        /// Check whether this object is an instance of the given type
        bool is_a(const python::Class& type) const;

        /// Check whether this object is an instance of the given type
        template<typename T>
        bool is_a() const;

        /// Is specified method overridden in Python?
        bool is_overridden(const char* name);
        /// Is specified method overridden in Python?
        bool is_overridden(const Object& method);

        /// Increases the reference count of the object
        void addref();
        /// Decreases the reference count of the object
        void release();

        /// Returns the wrapped PyObject*
        PyObject* ptr();

        template<typename T>
        T attribute(const char* name) const;

        template<typename T>
        void set_attribute(const char* name, const T& attr);

        template<int N>
        void set_attribute(const char* name, const char(&value)[N]);

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

    template<typename TClass>
    TClass* object_cast(python::BaseObject* object);
}

#include "BaseObject.inl"

#endif // __PYTHON_BASE_OBJECT_H__
