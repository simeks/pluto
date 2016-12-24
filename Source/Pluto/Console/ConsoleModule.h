#ifndef __CONSOLE_MODULE_H__
#define __CONSOLE_MODULE_H__

#include <Core/Python/PythonModule.h>

class ConsoleWidget;
class ConsoleModule : public PythonModule
{
public:
    ConsoleModule(ConsoleWidget* widget);
    virtual ~ConsoleModule();

    virtual void init_module() OVERRIDE;

    void focus() const;
    void set_style_sheet(const char* stylesheet);

private:
    ConsoleWidget* _widget;
};

#endif // __CONSOLE_MODULE_H__
