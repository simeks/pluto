#ifndef __CORE_PYTHON_FUNCTION_H__
#define __CORE_PYTHON_FUNCTION_H__

#include "PythonCommon.h"
#include "PythonHelpers.h"

#define PYTHON_FUNCTION_NAME_CLASS(TClass, Name) _py_wrapper_##TClass##_##Name

#define PYTHON_FUNCTION_CATCH_ERROR() \
    if (PyErr_Occurred()) \
        return 0;

#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(TClass, Fn) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        tself->Fn(); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        Py_RETURN_NONE; \
    }

#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(TClass, Fn, A) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* args, PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; \
        python_helpers::parse_args(args, a); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        tself->Fn(a); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        Py_RETURN_NONE; \
    }
#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(TClass, Fn, A, B) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* args, PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; B b; \
        python_helpers::parse_args(args, a, b); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        tself->Fn(a, b); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        Py_RETURN_NONE; \
    }
#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS3(TClass, Fn, A, B, C) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* args, PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; B b; C c; \
        python_helpers::parse_args(args, a, b, c); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        tself->Fn(a, b, c); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        Py_RETURN_NONE; \
    }

#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(TClass, Fn) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        return python_convert::to_python(tself->Fn()); \
    }
#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(TClass, Fn, A) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; \
        python_helpers::parse_args(args, a); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        return python_convert::to_python(tself->Fn(a)); \
    }
#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2_RETURN(TClass, Fn, A, B) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; B b; \
        python_helpers::parse_args(args, a, b); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        return python_convert::to_python(tself->Fn(a, b)); \
    }
#define PYTHON_FUNCTION_WRAPPER_CLASS_ARGS3_RETURN(TClass, Fn, A, B, C) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Fn)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)pyobject_extract_instance<TClass>(self); \
        A a; B b; C c; \
        python_helpers::parse_args(args, a, b, c); \
        PYTHON_FUNCTION_CATCH_ERROR(); \
        return python_convert::to_python(tself->Fn(a, b, c)); \
    }

template<typename TClass, typename R>
void create_function(R TClass::*fn);

class PythonFunction
{
public:
    PythonFunction(
        void* self, 
        const char* name, 
        PyCFunction meth, 
        int flags,
        const char* doc);
    ~PythonFunction();

    PyObject* object();

private:
    PyMethodDef _def;
    PyCFunctionObject* _fn;
};


#endif // __CORE_PYTHON_FUNCTION_H__
