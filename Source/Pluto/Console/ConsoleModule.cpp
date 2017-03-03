#include <Core/Common.h>
#include <Core/Python/Module.h>
#include <Core/Python/PythonCommon.h>

#include "ConsoleModule.h"
#include "ConsoleWidget.h"

namespace py = python;

PYTHON_MODULE(console)
{
    py::def(module, "set_style_sheet", &console::set_style_sheet, "set_style_sheet(sheet)");
    py::def(module, "print_html", &console::print_html, "print_html(html)");
    py::def(module, "print_image", &console::print_image, "print_image(bytes)");
}
void console::install_python_module()
{
    PYTHON_MODULE_INSTALL(console);
}

namespace
{
    ConsoleWidget* _widget;
}

void console::set_widget(ConsoleWidget* widget)
{
    _widget = widget;
}
void console::print_html(const char* txt)
{
    if (_widget)
        QMetaObject::invokeMethod(_widget, "kernel_output", Q_ARG(QString, txt), Q_ARG(bool, true));
}
void console::print_image(const python::Object& obj)
{
    if (PyBytes_Check(obj.ptr()))
    {
        QImage img = QImage::fromData((const uchar*)PyBytes_AsString(obj.ptr()), (int)PyBytes_Size(obj.ptr()));
        img = img.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QMetaObject::invokeMethod(_widget, "append_image", Q_ARG(QImage, img));
    }
    else
    {
        PYTHON_ERROR(TypeError, "Expected a bytes object");
    }
}
void console::set_style_sheet(const char* stylesheet)
{
    if (_widget)
        QMetaObject::invokeMethod(_widget, "set_style_sheet", Q_ARG(QString, stylesheet));
}
