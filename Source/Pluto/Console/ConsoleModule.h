#ifndef __CONSOLE_MODULE_H__
#define __CONSOLE_MODULE_H__

class ConsoleWidget;

namespace python
{
    class Object;
}

namespace console
{
    void set_widget(ConsoleWidget* widget);

    void print_html(const char* txt);

    /// @param obj : bytes object
    void print_image(const python::Object& obj);

    void set_style_sheet(const char* stylesheet);

    void install_python_module();
}

#endif // __CONSOLE_MODULE_H__
