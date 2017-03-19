#include "Common.h"

#include "Error.h"
#include "Function.h"
#include "Object.h"

#include "PythonCommon.h"

/// Python function type based on PythonCFunction

namespace python
{
    typedef struct
    {
        PyObject_HEAD;
        const char* name;
        const char* doc;
        function::CallerBase* caller;
    } Function;
}

static PyObject* function_new(PyTypeObject* type, PyObject*, PyObject*)
{
    python::Function* obj = (python::Function*)type->tp_alloc(type, 0);
    obj->name = nullptr;
    obj->doc = nullptr;
    obj->caller = nullptr;
    
    return (PyObject*)obj;
}
static void function_dealloc(python::Function* self)
{
    delete self->caller;
    Py_TYPE(self)->tp_free(self);
}
static PyObject* function_call(
    python::Function* f,
    PyObject* args,
    PyObject* kw)
{
    try
    {
        PyObject* ret = (*f->caller)(args, kw);
        if (PyErr_Occurred())
            return nullptr;
        return ret;
    }
    catch (const python::ErrorSet&)
    {
        return nullptr;
    }
    catch (const std::exception& e)
    {
        PYTHON_ERROR(PyExc_RuntimeError, "std::exception: %s", e.what());
    }
    catch (...)
    {
        PYTHON_ERROR(PyExc_RuntimeError, "Unknown C++ exception");
    }
}

static PyObject* function_get__doc__(python::Function* f, void*)
{
    if (f->doc)
        return _PyType_GetDocFromInternalDoc(f->name, f->doc);
    Py_RETURN_NONE;
}

static PyObject* function_get__name__(python::Function* f, void*)
{
    return PyUnicode_FromString(f->name);
}
static PyObject* function_descr_get(PyObject* func, PyObject* obj, PyObject*)
{
    if (obj == Py_None || obj == NULL) 
    {
        Py_INCREF(func);
        return func;
    }
    return PyMethod_New(func, obj);
}
static PyGetSetDef function_getsets[] = {
    { "__doc__",  (getter)function_get__doc__,  NULL, NULL },
    { "__name__", (getter)function_get__name__, NULL, NULL },
    { 0 }
};

static PyTypeObject PythonFunction_Type =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "pluto_function_or_method",
    sizeof(python::Function),
    0,
    (destructor)function_dealloc,                   /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    (ternaryfunc)function_call,                     /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,    /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    0,                                          /* tp_members */
    function_getsets,                           /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    function_descr_get,                         /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    PyType_GenericAlloc,                        /* tp_alloc */
    function_new,                               /* tp_new */
};

namespace python
{
    Object make_function2(function::CallerBase* caller, const char* name, const char* doc)
    {
        if (Py_TYPE(&PythonFunction_Type) == 0)
            PyType_Ready(&PythonFunction_Type);

        PyObject* args = PyTuple_New(0);
        python::Function* fn = (python::Function*)PyObject_Call((PyObject*)&PythonFunction_Type, args, nullptr);
        Py_DECREF(args);

        fn->caller = caller;
        fn->name = name;
        fn->doc = doc;

        return (PyObject*)fn;
    }
}
