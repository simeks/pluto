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

    PyObject* version();
    PyObject* print_html(const Tuple& args);

private:
    PlutoModuleCallback* _callback;
};

#endif // __CORE_PLUTO_MODULE_H__
