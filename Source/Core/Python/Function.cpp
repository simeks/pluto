#include "Common.h"

#include "Function.h"
#include "Object.h"

/// Python function type based on PythonCFunction

namespace python
{
    struct Function : PyObject
    {
        Function(std::unique_ptr<function::CallerBase> caller) : _caller(std::move(caller)) {}
        ~Function() {}

        PyObject* call(PyObject* args, PyObject* kw)
        {
            PyObject* ret = (*_caller)(args, kw);
            if (PyErr_Occurred())
                return nullptr;
            return ret;
        }

        std::unique_ptr<function::CallerBase> _caller;
    };
}


static void func_dealloc(python::Function* f)
{
    delete f;
}
static PyObject* func_call(
    python::Function* f,
    PyObject* args,
    PyObject* kw)
{
    try
    {
        return f->call(args, kw);
    }
    catch (const std::exception& e)
    {
        PYTHON_ERROR_R(RuntimeError, nullptr, "std::exception: %s", e.what());
    }
    catch (...)
    {
        PYTHON_ERROR_R(RuntimeError, nullptr, "Unknown C++ exception");
    }
}

static PyTypeObject PythonFunction_Type =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "pluto_function_or_method",
    sizeof(python::Function),
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
    (ternaryfunc)func_call,                     /* tp_call */
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
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
};

namespace python
{
    Object make_function(std::unique_ptr<function::CallerBase> caller)
    {
        if (Py_TYPE(&PythonFunction_Type) == 0)
            PyType_Ready(&PythonFunction_Type);

        PyObject* fn = new Function(std::move(caller));
        PyObject_INIT(fn, &PythonFunction_Type);

        Object obj(fn);
        Py_DECREF(fn); // TODO: Remove redundant reference handling

        return obj;
    }
}