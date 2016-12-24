#include <Core/Common.h>
#include <Core/Python/PythonFunction.h>

#include "ConsoleModule.h"
#include "ConsoleWidget.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(ConsoleModule, focus);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ConsoleModule, set_style_sheet, const char*);


ConsoleModule::ConsoleModule(ConsoleWidget* widget) : 
    PythonModule("console"),
    _widget(widget)
{
}
ConsoleModule::~ConsoleModule()
{
}
void ConsoleModule::init_module()
{
    PythonModule::init_module();

    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, focus, "");
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, set_style_sheet, "");
}
void ConsoleModule::focus() const
{
    QMetaObject::invokeMethod(_widget, "setFocus");
}
void ConsoleModule::set_style_sheet(const char* stylesheet)
{
    QMetaObject::invokeMethod(_widget, "set_style_sheet", Q_ARG(QString, stylesheet));
}



