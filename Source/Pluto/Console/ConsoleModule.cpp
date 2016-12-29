#include <Core/Common.h>
#include <Core/Python/PythonFunction.h>

#include "ConsoleModule.h"
#include "ConsoleWidget.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(ConsoleModule, focus);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ConsoleModule, set_style_sheet, const char*);


ConsoleModule::ConsoleModule() : _widget(nullptr)
{
}
ConsoleModule::~ConsoleModule()
{
}
void ConsoleModule::set_widget(ConsoleWidget* widget)
{
    _widget = widget;
}
void ConsoleModule::post_init()
{
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, focus, "");
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, set_style_sheet, "");
}
void ConsoleModule::focus() const
{
    if (_widget)
        QMetaObject::invokeMethod(_widget, "setFocus");
}
void ConsoleModule::set_style_sheet(const char* stylesheet)
{
    if (_widget)
        QMetaObject::invokeMethod(_widget, "set_style_sheet", Q_ARG(QString, stylesheet));
}
const char* ConsoleModule::name()
{
    return "console";
}
