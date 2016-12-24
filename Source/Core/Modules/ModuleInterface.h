#ifndef __CORE_MODULE_H__
#define __CORE_MODULE_H__

#define PLUTO_IMPLEMENT_MODULE(ModuleImpl) \
    extern "C" API_EXPORT ModuleInterface* create_module() \
    { \
        return new ModuleImpl();\
    }


class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual void install() = 0;
    virtual void uninstall() = 0;
};

#endif // __CORE_MODULE_H__
