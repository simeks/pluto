#ifndef __CORE_IMAGE_MODULE_H__
#define __CORE_IMAGE_MODULE_H__

#include <Core/Python/PythonModule.h>

class ImageModule : public PythonModule
{
public:
    ImageModule();
    virtual ~ImageModule();

    virtual void create() OVERRIDE;

};

#endif // __CORE_IMAGE_MODULE_H__
