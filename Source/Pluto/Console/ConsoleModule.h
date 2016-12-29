#ifndef __CONSOLE_MODULE_H__
#define __CONSOLE_MODULE_H__

#include <Core/Python/PythonModule.h>


class ConsoleWidget;
class ConsoleModule : public PythonModuleHelper<ConsoleModule>
{
public:
    ConsoleModule();
    virtual ~ConsoleModule();

    void set_widget(ConsoleWidget* widget);

    void post_init() OVERRIDE;

    void focus() const;
    void set_style_sheet(const char* stylesheet);

    static const char* name();
private:
    static ConsoleModule* s_instance;

    ConsoleWidget* _widget;
};

#endif // __CONSOLE_MODULE_H__
