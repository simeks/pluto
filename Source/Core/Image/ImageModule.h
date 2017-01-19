#ifndef __CORE_IMAGE_MODULE_H__
#define __CORE_IMAGE_MODULE_H__

#include <Core/Python/PythonModule.h>

#include "Types.h"

class ImageModule : public PythonModuleHelper<ImageModule>
{
public:
    ImageModule();
    virtual ~ImageModule();

    virtual void post_init() OVERRIDE;

    const char* type_to_string(image::PixelType type);
    image::PixelType type_from_string(const char* str);

    static const char* name();
};

#endif // __CORE_IMAGE_MODULE_H__
