#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

#include <Core/Python/PythonModule.h>

class PlutoModuleCallback;
class PyStdStream;
class PlutoModule : public PythonModule
{
public:
    PlutoModule(PlutoModuleCallback* callback);
    virtual ~PlutoModule();

    virtual void create() OVERRIDE;

    const char* version();
    void print_html(const std::string& txt);

private:
    PlutoModuleCallback* _callback;
};

#endif // __CORE_PLUTO_MODULE_H__
