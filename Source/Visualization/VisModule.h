#ifndef __VIS_MODULE_H__
#define __VIS_MODULE_H__

#include "API.h"

#include <Core/Modules/ModuleInterface.h>

class VIS_API VisModule : public ModuleInterface
{
public:
    VisModule();
    ~VisModule();

    void install() OVERRIDE;
    void uninstall() OVERRIDE;

    void init() OVERRIDE;

    static VisModule& instance();

private:
    static VisModule* s_instance;
};



#endif // __VIS_MODULE_H__
