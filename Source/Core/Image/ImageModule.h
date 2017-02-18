#ifndef __CORE_IMAGE_MODULE_H__
#define __CORE_IMAGE_MODULE_H__


#include "Types.h"

namespace image
{
    const char* type_to_string(image::PixelType type);
    image::PixelType type_from_string(const char* str);

    void install_python_module();
}

#endif // __CORE_IMAGE_MODULE_H__
