#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__

#include "Class.h"
#include <Core/Python/Convert.h>

template<typename TClass, typename M, typename R, typename A>
static PyObject* _py_method_wrapper_noargs(PyObject* self, PyObject* , PyObject*)
{
    TClass* tself = (TClass*)py_object::object(self);
    return tself->M();
}
template<typename TClass, typename M, typename R, typename A>
static PyObject* _py_method_wrapper_varargs(PyObject* self, PyObject* args, PyObject* )
{
    TClass* tself = (TClass*)py_object::object(self);
    return tself->M(Tuple(args));
}

#define OBJECT_INIT_TYPE_FN_NAME(TClass) TClass##_init_type

#define OBJECT_INIT_TYPE_FN(TClass) \
    static void OBJECT_INIT_TYPE_FN_NAME(TClass)(PythonType* type)

#define OBJECT_PYTHON_METHOD_NAME(TClass, Name) _py_wrapper_##TClass##_##Name

#define OBJECT_PYTHON_NOARGS_METHOD_IMPL(TClass, Name) \
    static PyObject* OBJECT_PYTHON_METHOD_NAME(TClass, Name)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)py_object::object(self); \
        return tself->Name(); \
    }

#define OBJECT_PYTHON_VARARGS_METHOD_IMPL(TClass, Name) \
    static PyObject* OBJECT_PYTHON_METHOD_NAME(TClass, Name)(PyObject* self, PyObject* args, PyObject* ) \
    { \
        TClass* tself = (TClass*)py_object::object(self); \
        return tself->Name(Tuple(args)); \
    }

#define OBJECT_PYTHON_KEYWORDS_METHOD_IMPL(TClass, Name) \
    static PyObject* OBJECT_PYTHON_METHOD_NAME(TClass, Name)(PyObject* self, PyObject* args, PyObject* kw) \
    { \
        TClass* tself = (TClass*)py_object::object(self); \
        return tself->Name(Tuple(args), Dict(kw)); \
    }

#define OBJECT_PYTHON_NO_METHODS() type;

#define OBJECT_PYTHON_ADD_METHOD(TClass, Name, Flags, Doc) \
    type->add_method(#Name, (PyCFunction)OBJECT_PYTHON_METHOD_NAME(TClass, Name), Flags, Doc);

#define OBJECT_PYTHON_ADD_METHOD_NOARGS(TClass, Name, Flags, Doc) \
    type->add_method(#Name, (PyCFunction)_py_method_wrapper_noargs<TClass, Name, int, int>((TClass, Name), Flags, Doc);

#define OBJECT_PYTHON_ADD_METHOD_VARARGS(TClass, Name, Flags, Doc) \
    type->add_method(#Name, (PyCFunction)_py_method_wrapper_varargs<TClass, Name, int, int>((TClass, Name), Flags, Doc);


#define OBJECT_FROM_PYTHON(T) \
    template<> \
    CORE_API T* from_python(PyObject* obj) \
    { \
        if (Object::python_type()->check_type(obj)) \
        { \
            Object* ret = py_object::object(obj); \
            if (ret->is_a(T::static_class())) \
            { \
                return object_cast<T>(ret); \
            } \
        } \
        PyErr_SetString(PyExc_ValueError, "Failed to convert Object"); \
        return nullptr; \
    }


#define DECLARE_OBJECT(TClass, TSuperClass) \
    public: \
    typedef TSuperClass Super; \
    typedef TClass ThisClass; \
    \
    static Object* create_object(); \
    static Object* create_object(PyObject* pyobj); \
    static PythonType* python_type(); \
    static Class* static_class(); \
    virtual Class* get_class() const; \
    virtual void copy_object_to(Object* object) const; \
    virtual PyObject* python_object(); 

#define IMPLEMENT_OBJECT(TClass, Name) \
    namespace python_convert \
    { \
        OBJECT_FROM_PYTHON(TClass); \
    } \
    static PyObject* py_##TClass##_new(PyTypeObject* type, PyObject* args, PyObject*) \
    { \
        PyObject* pyobj = type->tp_alloc(type, 0); \
        if (PyTuple_Check(args) && \
            PyTuple_Size(args) == 1) \
        { \
            PyObject* cap = PyTuple_GetItem(args, 0); \
            if (PyCapsule_CheckExact(cap)) \
            { \
                py_object::set_object(pyobj, (Object*)PyCapsule_GetPointer(cap, "object")); \
                return pyobj; \
            } \
        } \
        py_object::set_object(pyobj, TClass::create_object(pyobj)); \
        return pyobj; \
    } \
    static int py_##TClass##_init(PyObject* self, PyObject* arg, PyObject* kw) \
    { \
        if (!PyCapsule_CheckExact(arg)) \
        {\
            return py_object::object(self)->object_init(Tuple(arg), Dict(kw)); \
        }\
        return 0; \
    } \
    static void py_##TClass##_dealloc(PyObject* self) \
    { \
        Object* obj = py_object::object(self); \
        if (obj) \
        { \
            delete obj; \
        } \
    } \
    Object* TClass::create_object() \
    { \
        return new TClass(); \
    } \
    Object* TClass::create_object(PyObject* pyobj) \
    { \
        TClass* obj = new TClass(); \
        obj->set_python_object(pyobj); \
        return obj; \
    } \
    PythonType* TClass::python_type() \
    { \
        static PythonType* type = nullptr; \
        if (!type) \
        { \
            type = py_object::register_python_type(Name);\
        } \
        return type; \
    } \
    static Class* _##TClass##_type = TClass::static_class(); \
    Class* TClass::static_class() \
    { \
        static Class* type = nullptr; \
        if (!type) \
        { \
            type = new Class(Name, sizeof(TClass), TClass::create_object); \
            if (type != TClass::Super::static_class()) \
            { \
                type->set_super(TClass::Super::static_class()); \
            } \
            PythonType* py_type = TClass::python_type(); \
            py_type->set_new(py_##TClass##_new); \
            py_type->set_init(py_##TClass##_init); \
            py_type->set_dealloc(py_##TClass##_dealloc); \
            py_type->set_base(Super::python_type()); \
            OBJECT_INIT_TYPE_FN_NAME(TClass)(py_type); \
        } \
        return type; \
    } \
    Class* TClass::get_class() const \
    { \
        return TClass::static_class(); \
    } \
    void TClass::copy_object_to(Object* object) const \
    { \
        if (object->get_class() == TClass::static_class()) \
        { \
            *static_cast<TClass*>(object) = *this; \
        } \
    } \
    PyObject* TClass::python_object() \
    { \
        if (!_py_object) \
        { \
            PyObject* args = PyTuple_New(1); \
            PyTuple_SetItem(args, 0, PyCapsule_New(this, "object", nullptr)); \
            _py_object = TClass::python_type()->create_object(args, nullptr); \
        } \
        return _py_object; \
    }

class Dict;
class PythonType;
class Tuple;

class CORE_API Object
{
    DECLARE_OBJECT(Object, Object);

public:
    Object();
    virtual ~Object();

    Object* clone() const;

    bool is_a(Class* type) const;

    template<typename T>
    bool is_a() const { return is_a(T::static_class()); }

    Object(const Object& other);
    Object& operator=(const Object& other);

    void addref();
    void release();

    PyObject* object_type();

    PyObject* invoke_method(const char* name, const Tuple& args);
    PyObject* invoke_method(const char* name, PyObject* args);

    virtual int object_init(const Tuple&, const Dict&) { return 0; }
protected:
    void set_python_object(PyObject* obj);
    void validate();

    PyObject* _py_object;

};

template<typename Type>
Type* object_cast(Object* object)
{
    if (!object || !object->is_a(Type::static_class()))
        return nullptr;

    return (Type*)object;
}

namespace py_object
{
    PythonType* register_python_type(const char* name);

    void set_object(PyObject* self, Object* obj);
    Object* object(PyObject* self);
}

#endif // __CORE_OBJECT_H__
