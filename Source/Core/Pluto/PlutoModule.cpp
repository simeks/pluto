#include "Common.h"

#include "Image/ImageObject.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/PythonFunction.h"
#include "Python/StdStream.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(PlutoModule, get_user_dir);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(PlutoModule, print_html, std::string);

std::string PlutoModule::s_version = "0.0";

PlutoModule::PlutoModule() : PythonModule("pluto")
{
}
PlutoModule::~PlutoModule()
{
}
void PlutoModule::init_module()
{
    PythonModule::init_module();

    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, get_user_dir, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, print_html, "");

    add_type("Object", Object::static_class());
    add_type("StdStream", PyStdStream::static_class());
    add_object("__version__", python_convert::to_python(s_version));
}
const char* PlutoModule::get_user_dir()
{
    return "..\\User"; // TODO:
}
void PlutoModule::print_html(const std::string& txt)
{
    PyStdStream* htmlout = object<PyStdStream>("htmlout");
    if (htmlout)
        htmlout->write(txt.c_str());
}

