#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__

#include "Class.h"
#include "PythonClass.h"
#include <Core/Python/PythonFunction.h>


#define OBJECT_INIT_TYPE_FN_NAME(TClass) TClass##_init_type

#define OBJECT_INIT_TYPE_FN(TClass) \
    static void OBJECT_INIT_TYPE_FN_NAME(TClass)(PythonClass* type)

#define OBJECT_PYTHON_METHOD_NAME(TClass, Name) _py_wrapper_##TClass##_##Name

#define OBJECT_PYTHON_NO_METHODS() type;

#define OBJECT_PYTHON_ADD_METHOD(TClass, Name, Doc) \
    type->add_method(#Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), METH_VARARGS, Doc);

#define OBJECT_PYTHON_ADD_CLASS_ATTR(Name, Value) \
    type->add_attr(Name, python_convert::to_python(Value));

#define OBJECT_CONVERTER_FROM_PYTHON(TClass, API) \
    namespace python_convert { \
        template<> \
        API TClass* from_python(PyObject* obj) { \
            if (TClass::static_class()->check_type(obj)) { \
                Object* ret = python_object::object(obj); \
                if (ret->is_a(TClass::static_class())) { \
                    return object_cast<TClass>(ret); \
                } \
            } \
            PyErr_SetString(PyExc_ValueError, "Failed to convert Object"); \
            return nullptr; \
        } \
    }
#define OBJECT_CONVERTER_TO_PYTHON(TClass, API) \
    namespace python_convert { \
        template<> \
        API PyObject* to_python(TClass* const& obj) { \
            if (obj) return obj->python_object(); \
            return nullptr; \
        } \
    }

#define DECLARE_OBJECT(TClass, TSuperClass) \
    public: \
    typedef TSuperClass Super; \
    typedef TClass ThisClass; \
    \
    static Object* create_object(); \
    static PythonClass* static_class(); \
    virtual PythonClass* get_class(); \
    virtual Object* clone(); \
    virtual Object* clone(PyObject* pyobj);

#define IMPLEMENT_OBJECT(TClass, Name, API) \
    OBJECT_CONVERTER_FROM_PYTHON(TClass, API); \
    OBJECT_CONVERTER_TO_PYTHON(TClass, API); \
    Object* TClass::create_object() \
    { \
        return new TClass(); \
    } \
    static PythonClass* _##TClass##_type = TClass::static_class(); \
    PythonClass* TClass::static_class() \
    { \
        static PythonClass* type = nullptr; \
        if (!type) \
        { \
            type = new PythonClass(Name, sizeof(TClass), TClass::create_object); \
            if (type != TClass::Super::static_class()) \
            { \
                type->set_super(TClass::Super::static_class()); \
            } \
            OBJECT_INIT_TYPE_FN_NAME(TClass)(type); \
        } \
        return type; \
    } \
    PythonClass* TClass::get_class() \
    { \
        if (!_class) \
            _class = TClass::static_class(); \
        return _class; \
    } \
    Object* TClass::clone() \
    { \
        return object_clone<TClass>(this); \
    } \
    Object* TClass::clone(PyObject* pyobj)   \
    { \
        return object_clone<TClass>(this, pyobj); \
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

    bool is_a(Class* type);

    template<typename T>
    bool is_a() { return is_a(T::static_class()); }

    Object(const Object& other);
    Object& operator=(const Object& other);

    void addref();
    void release();

    PyObject* object_type();

    PyObject* invoke_method(const char* name, PyObject* args);

    template<typename R, typename A>
    R invoke_method(const char* name, const A& a);
    template<typename R, typename A, typename B>
    R invoke_method(const char* name, const A& a, const B& b);
    template<typename R, typename A, typename B, typename C>
    R invoke_method(const char* name, const A& a, const B& b, const C& c);

    void set_attribute(const char* name, PyObject* attr);

    bool has_attribute(const char* name) const;
    PyObject* attribute(const char* name) const;

    template<typename T>
    void set_attribute(const char* name, const T& attr);

    template<typename T>
    T attribute(const char* name) const;

    virtual int object_init(const Tuple&, const Dict&) { return 0; }

    void set_class(PythonClass* cls);

    PyObject* python_object();
    void set_python_object(PyObject* obj);

protected:
    PythonClass* _class;
    PyObject* _py_object;
};

template<typename R, typename A>
R Object::invoke_method(const char* name, const A& a)
{
    return python_convert::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}
template<typename R, typename A, typename B>
R Object::invoke_method(const char* name, const A& a, const B& b)
{
    return python_convert::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}
template<typename R, typename A, typename B, typename C>
R Object::invoke_method(const char* name, const A& a, const B& b, const C& c)
{
    return python_convert::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}


template<typename T>
void Object::set_attribute(const char* name, const T& attr)
{
    set_attribute(name, python_convert::to_python<T>(attr));
}
template<typename T>
T Object::attribute(const char* name) const
{
    return python_convert::from_python<T>(attribute(name));
}

template<typename Type>
Type* object_cast(Object* object)
{
    if (!object || !object->is_a(Type::static_class()))
        return nullptr;

    return (Type*)object;
}

template<typename Type>
Type* object_new()
{
    Type* o = new Type();
    o->set_python_object(Type::static_class()->create_python_object(o));
    return o;
}
template<typename Type, typename Arg0>
Type* object_new(Arg0 arg0)
{
    Type* o = new Type(arg0);
    o->set_python_object(Type::static_class()->create_python_object(o));
    return o;
}
template<typename Type, typename Arg0, typename Arg1>
Type* object_new(Arg0 arg0, Arg1 arg1)
{
    Type* o = new Type(arg0, arg1);
    o->set_python_object(Type::static_class()->create_python_object(o));
    return o;
}
template<typename Type, typename Arg0, typename Arg1, typename Arg2>
Type* object_new(Arg0 arg0, Arg1 arg1, Arg2 arg2)
{
    Type* o = new Type(arg0, arg1, arg2);
    o->set_python_object(Type::static_class()->create_python_object(o));
    return o;
}
template<typename Type, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
Type* object_new(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    Type* o = new Type(arg0, arg1, arg2, arg3);
    o->set_python_object(Type::static_class()->create_python_object(o));
    return o;
}
template<typename Type>
Type* object_clone(Type* obj)
{
    Type* o = new Type(*obj);
    o->set_python_object(python_object::copy(obj->python_object(), o));
    return o;
}
template<typename Type>
Type* object_clone(Type* obj, PyObject* pyobj)
{
    Type* o = new Type(*obj);
    o->set_python_object(pyobj);
    return o;
}


#endif // __CORE_OBJECT_H__
