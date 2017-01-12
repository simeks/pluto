#ifndef __CORE_MODULE_H__
#define __CORE_MODULE_H__

#define PLUTO_IMPLEMENT_MODULE(ModuleImpl) \
    extern "C" API_EXPORT ModuleInterface* create_module() \
    { \
        return new ModuleImpl();\
    }


class CORE_API ModuleInterface
{
public:
    virtual ~ModuleInterface() {}

    /// Called right after the module has been loaded
    virtual void install() {};
    virtual void uninstall() {};

    /// Called after the core has been initialized 
    virtual void init() {}
    /// Called before core shutdown
    virtual void shutdown() {}
};

#endif // __CORE_MODULE_H__
