#include "Common.h"

#include "Image/ImageObject.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/PythonFunction.h"
#include "Python/StdStream.h"

PYTHON_NOARGS_FUNCTION_IMPL_CLASS(PlutoModule, version);
PYTHON_VARARGS_FUNCTION_IMPL_CLASS(PlutoModule, print_html);


PlutoModule::PlutoModule(PlutoModuleCallback* callback) : PythonModule("pluto"), _callback(callback)
{
}
PlutoModule::~PlutoModule()
{
}
void PlutoModule::create()
{
    PythonModule::create();

    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, version, METH_NOARGS, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, print_html, METH_VARARGS, "");

    add_type("Object", Object::python_type());
    add_type("StdStream", PyStdStream::python_type());
    add_type("Image", ImageObject::python_type());
}
PyObject* PlutoModule::version()
{
    return PyUnicode_FromString("PrePrePre");
}
PyObject* PlutoModule::print_html(const Tuple& args)
{
    if (args.size() != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Expecting only 1 argument");
        return nullptr;
    }

    if (_callback)
    {
        if (!PyUnicode_Check(args.get(0)))
        {
            PyErr_SetString(PyExc_TypeError, "Expecting str");
            return nullptr;
        }
        _callback->print_html(PyUnicode_AsUTF8(args.get(0)));
    }
    Py_RETURN_NONE;
}
