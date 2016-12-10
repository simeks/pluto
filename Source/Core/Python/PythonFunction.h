#ifndef __CORE_PYTHON_FUNCTION_H__
#define __CORE_PYTHON_FUNCTION_H__

#include "PythonCommon.h"

#define PYTHON_FUNCTION_NAME_CLASS(TClass, Name) _py_wrapper_##TClass##_##Name

#define PYTHON_NOARGS_FUNCTION_IMPL_CLASS(TClass, Name) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Name)(PyObject* self, PyObject* , PyObject* ) \
    { \
        TClass* tself = (TClass*)PyCapsule_GetPointer(self, nullptr); \
        return tself->Name(); \
    }

#define PYTHON_VARARGS_FUNCTION_IMPL_CLASS(TClass, Name) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Name)(PyObject* self, PyObject* args, PyObject* ) \
    { \
        TClass* tself = (TClass*)PyCapsule_GetPointer(self, nullptr); \
        return tself->Name(Tuple(args)); \
    }

#define PYTHON_KEYWORDS_FUNCTION_IMPL_CLASS(TClass, Name) \
    static PyObject* PYTHON_FUNCTION_NAME_CLASS(TClass, Name)(PyObject* self, PyObject* args, PyObject* kw) \
    { \
        TClass* tself = (TClass*)PyCapsule_GetPointer(self, nullptr); \
        return tself->Name(Tuple(args), Dict(kw)); \
    }

#define PYTHON_CREATE_FUNCTION(TClass, Name, Flags, Doc) \
    python_function::create_function(this, #Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), Flags, Doc)


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
