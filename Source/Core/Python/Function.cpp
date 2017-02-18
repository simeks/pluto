#include "Common.h"

#include "Function.h"
#include "Object.h"

/// Python function type based on PythonCFunction

namespace python
{
    struct Function : PyObject
    {
        Function(std::unique_ptr<function::CallerBase> caller, 
            const char* name, const char* doc) 
            : _caller(std::move(caller)), _name(name), _doc(doc) {}
        ~Function() {}

        PyObject* call(PyObject* args, PyObject* kw)
        {
            PyObject* ret = (*_caller)(args, kw);
            if (PyErr_Occurred())
                return nullptr;
            return ret;
        }

        std::unique_ptr<function::CallerBase> _caller;

        const char* _name;
        const char* _doc;
    };
}


static void function_dealloc(python::Function* f)
{
    delete f;
}
static PyObject* function_call(
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

static PyObject* function_get__doc__(python::Function* f, void*)
{
    if (f->_doc)
        return _PyType_GetDocFromInternalDoc(f->_name, f->_doc);
    Py_RETURN_NONE;
}

static PyObject* function_get__name__(python::Function* f, void*)
{
    return PyUnicode_FromString(f->_name);
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
};

namespace python
{
    Object make_function(std::unique_ptr<function::CallerBase> caller, const char* name, const char* doc)
    {
        if (Py_TYPE(&PythonFunction_Type) == 0)
            PyType_Ready(&PythonFunction_Type);

        PyObject* fn = new Function(std::move(caller), name, doc);
        PyObject_INIT(fn, &PythonFunction_Type);

        Object obj(fn);
        Py_DECREF(fn); // TODO: Remove redundant reference handling

        return obj;
    }
}
