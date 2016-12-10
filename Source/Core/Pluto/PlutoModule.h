#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

#include "Python/PythonModule.h"

class PyStdStream;
class PlutoModule : public PythonModule
{
public:
    PlutoModule();
    virtual ~PlutoModule();

    virtual void create() OVERRIDE;

    PyObject* version();

};

#endif // __CORE_PLUTO_MODULE_H__
