#ifndef __REGISTRATION_MODULE_H__
#define __REGISTRATION_MODULE_H__

#include <Core/Modules/ModuleInterface.h>

class RegistrationModule : public ModuleInterface
{
public:
    RegistrationModule();
    ~RegistrationModule();

    void install() OVERRIDE;
};

#endif // __REGISTRATION_MODULE_H__
