#include <Core/Common.h>
#include <Core/Python/PythonFunction.h>

#include "ConsoleModule.h"
#include "ConsoleWidget.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ConsoleModule, set_style_sheet, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ConsoleModule, print_html, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ConsoleModule, print_image, PyObject*);


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
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, set_style_sheet, "set_style_sheet(sheet)");
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, print_html, "print_html(html)");
    MODULE_ADD_PYTHON_FUNCTION(ConsoleModule, print_image, "print_image(bytes)");
}
void ConsoleModule::print_html(const char* txt)
{
    if (_widget)
        QMetaObject::invokeMethod(_widget, "kernel_output", Q_ARG(QString, txt), Q_ARG(bool, true));
}
void ConsoleModule::print_image(PyObject* obj)
{
    if (PyBytes_Check(obj))
    {
        QImage img = QImage::fromData((const uchar*)PyBytes_AsString(obj), (int)PyBytes_Size(obj));
        img = img.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QMetaObject::invokeMethod(_widget, "append_image", Q_ARG(QImage, img));
    }
    else
    {
        PYTHON_ERROR(TypeError, "Expected a bytes object");
    }
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
