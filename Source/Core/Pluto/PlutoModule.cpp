#include "Common.h"

#include "PlutoModule.h"

#include "Python/PythonFunction.h"
#include "Python/StdStream.h"

PYTHON_NOARGS_FUNCTION_IMPL_CLASS(PlutoModule, version);


PlutoModule::PlutoModule() : PythonModule("pluto")
{
}
PlutoModule::~PlutoModule()
{
}
void PlutoModule::create()
{
    PythonModule::create();

    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, version, METH_NOARGS, "");

    add_type("Object", Object::python_type());
    add_type("StdStream", PyStdStream::python_type());
}
PyObject* PlutoModule::version()
{
    return PyUnicode_FromString("PrePrePre");
}
