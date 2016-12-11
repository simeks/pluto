#include "Common.h"

#include "Image/ImageObject.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/PythonFunction.h"
#include "Python/StdStream.h"

PYTHON_FUNCTION_IMPL_CLASS_ARGS0_RETURN(PlutoModule, version);
PYTHON_FUNCTION_IMPL_CLASS_ARGS1(PlutoModule, print_html, std::string);


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
const char* PlutoModule::version()
{
    return "PrePrePre";
}
void PlutoModule::print_html(const std::string& txt)
{
    _callback->print_html(txt.c_str());
}
