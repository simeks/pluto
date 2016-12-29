#ifndef __CORE_IMAGE_MODULE_H__
#define __CORE_IMAGE_MODULE_H__

#include <Core/Python/PythonModule.h>

class ImageModule : public PythonModuleHelper<ImageModule>
{
public:
    ImageModule();
    virtual ~ImageModule();

    virtual void post_init() OVERRIDE;

    static const char* name();
};

#endif // __CORE_IMAGE_MODULE_H__
