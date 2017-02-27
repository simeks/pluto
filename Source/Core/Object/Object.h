#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__


#include <Core/Object/PythonClass.h>
#include <Core/Python/PythonCommon.h>


#define OBJECT_INIT_TYPE_FN_NAME(TClass) TClass##_init_type

#define OBJECT_INIT_TYPE_FN(TClass) \
    static void OBJECT_INIT_TYPE_FN_NAME(TClass)(PythonClass* type)

#define OBJECT_PYTHON_METHOD_NAME(TClass, Name) _py_wrapper_##TClass##_##Name

#define OBJECT_PYTHON_NO_METHODS() type;

#define OBJECT_PYTHON_ADD_METHOD(TClass, Name, Doc) \
    type->add_method(#Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), METH_VARARGS, PyDoc_STR(Doc));

#define OBJECT_PYTHON_ADD_KEYWORD_METHOD(TClass, Name, Doc) \
    type->add_method(#Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), METH_VARARGS|METH_KEYWORDS, PyDoc_STR(Doc));

#define OBJECT_PYTHON_ADD_CLASS_ATTR(Name, Value) \
    type->add_attr(Name, python::to_python(Value));


template<typename TClass>
struct ObjectPythonConverter
{
    static PyObject* to_python(TClass* const& obj)
    {
        if (obj) return obj->python_object();
        Py_RETURN_NONE;
    }
    static TClass* from_python(PyObject* obj)
    {
        if (obj == Py_None)
        {
            return nullptr;
        }
        if (TClass::static_class()->check_type(obj))
        {
            ::Object* ret = python_object::object(obj);
            if (ret->is_a(TClass::static_class()))
            {
                return object_cast<TClass>(ret);
            }
        }
        PyErr_SetString(PyExc_ValueError, "Failed to convert Object");
        return nullptr;
    }
};

#define OBJECT_CONVERTER(TClass) \
    namespace { \
        python::TypeConverter<::TClass*, ObjectPythonConverter<::TClass>> _##TClass##_converter; \
    }

#define DECLARE_OBJECT(TClass, TSuperClass) \
    public: \
    typedef TSuperClass Super; \
    typedef TClass ThisClass; \
    \
    static Object* create_object(PyObject*, PythonClass*); \
    static PythonClass* static_class(); \
    virtual PythonClass* get_class(); \
    virtual Object* clone();


#define IMPLEMENT_OBJECT_DOC(TClass, Name, API, Doc) \
    OBJECT_CONVERTER(TClass); \
    Object* TClass::create_object(PyObject* pyobj, PythonClass* cls) \
    { \
        return new TClass(pyobj, cls); \
    } \
    static PythonClass* _##TClass##_type = TClass::static_class(); \
    PythonClass* TClass::static_class() \
    { \
        static PythonClass* type = nullptr; \
        if (!type) \
        { \
            type = new PythonClass(Name, sizeof(TClass), TClass::create_object, OBJECT_INIT_TYPE_FN_NAME(TClass), PyDoc_STR(Doc)); \
            if (type != TClass::Super::static_class()) \
            { \
                type->set_super(TClass::Super::static_class()); \
            } \
        } \
        return type; \
    } \
    PythonClass* TClass::get_class() \
    { \
        if (_class) return _class; \
        return TClass::static_class(); \
    } \
    Object* TClass::clone() \
    { \
        return new TClass(*this); \
    }

#define IMPLEMENT_OBJECT(TClass, Name, API) IMPLEMENT_OBJECT_DOC(TClass, Name, API, "")

#define DECLARE_OBJECT_CONSTRUCTOR(TClass) \
    TClass(PyObject*, PythonClass* cls = TClass::static_class());

#define IMPLEMENT_OBJECT_CONSTRUCTOR(TClass, TSuperClass) \
    TClass::TClass(PyObject* pyobj, PythonClass* cls) : TSuperClass(pyobj, cls) {}

class Class;
class PythonClass;
class PythonType;

class CORE_API Object
{
    DECLARE_OBJECT(Object, Object);

public:
    Object(PyObject* pyobj, PythonClass* cls);
    Object(const Object& other);

    virtual ~Object();

    virtual void object_init() {}
    virtual void object_python_init(const Tuple&, const Dict&) {}

    bool is_a(Class* type);

    template<typename T>
    bool is_a() { return is_a(T::static_class()); }

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

    void set_attribute(const char* name, const python::Object& attr);

    bool has_attribute(const char* name) const;
    PyObject* attribute(const char* name) const;

    template<typename T>
    void set_attribute(const char* name, const T& attr);

    template<int N>
    void set_attribute(const char* name, const char(&value)[N]);

    template<typename T>
    T attribute(const char* name) const;

    PyObject* python_object();

    void set_class(PythonClass* cls);

protected:
    PythonClass* _class;
    PyObject* _py_object;

};

template<typename R, typename A>
R Object::invoke_method(const char* name, const A& a)
{
    return python::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}
template<typename R, typename A, typename B>
R Object::invoke_method(const char* name, const A& a, const B& b)
{
    return python::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}
template<typename R, typename A, typename B, typename C>
R Object::invoke_method(const char* name, const A& a, const B& b, const C& c)
{
    return python::from_python<R>(invoke_method(name, python_helpers::build_args(a)));
}


template<typename T>
void Object::set_attribute(const char* name, const T& attr)
{
    set_attribute(name, python::Object(python::to_python<T>(attr)));
}
template<int N>
void Object::set_attribute(const char* name, const char(&value)[N])
{
    set_attribute(name, PyUnicode_FromString(value));
}

template<typename T>
T Object::attribute(const char* name) const
{
    return python::from_python<T>(attribute(name));
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
    Type* o = (Type*)Type::static_class()->create_object();
    o->object_init();
    return o;
}
template<typename Type, typename Arg0>
Type* object_new(Arg0 arg0)
{
    Type* o = (Type*)Type::static_class()->create_object();
    o->object_init(arg0);
    return o;
}
template<typename Type, typename Arg0, typename Arg1>
Type* object_new(Arg0 arg0, Arg1 arg1)
{
    Type* o = (Type*)Type::static_class()->create_object();
    o->object_init(arg0, arg1);
    return o;
}
template<typename Type, typename Arg0, typename Arg1, typename Arg2>
Type* object_new(Arg0 arg0, Arg1 arg1, Arg2 arg2)
{
    Type* o = (Type*)Type::static_class()->create_object();
    o->object_init(arg0, arg1, arg2);
    return o;
}
template<typename Type, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
Type* object_new(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    Type* o = (Type*)Type::static_class()->create_object();
    o->object_init(arg0, arg1, arg2, arg3);
    return o;
}
template<typename Type>
Type* object_clone(Type* obj)
{
    return (Type*)obj->clone();
}


#endif // __CORE_OBJECT_H__
