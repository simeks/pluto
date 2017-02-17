#include "Common.h"

#include "PythonFunction.h"
#include "PythonCommon.h"

PythonFunction::PythonFunction(
    void* self, 
    const char* name, 
    PyCFunction meth, 
    int flags, 
    const char* doc)
{
    _def.ml_name = name;
    _def.ml_meth = meth;
    _def.ml_flags = flags;
    _def.ml_doc = doc;

    _fn = (PyCFunctionObject*)PyCFunction_New(&_def, PyCapsule_New(self, nullptr, nullptr));
    if (!_fn)
        PyErr_Print();
}
PythonFunction::~PythonFunction()
{
    Py_XDECREF(_fn);
}
PyObject* PythonFunction::object()
{
    return (PyObject*)_fn;
}


/// Python function type based on PythonCFunction

typedef struct {
    PyObject_HEAD
    
} PythonFunctionObject;

static void func_dealloc(PythonFunctionObject* f)
{
}
static PyObject* func_call(
    PythonFunctionObject* f,
    PyObject* arg,
    PyObject* kw)
{
}

static PyTypeObject PythonFunction_Type = 
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "pluto_function_or_method",
    sizeof(PythonFunctionObject),
    0,
    (destructor)func_dealloc,                   /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    func_call,                                  /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
};