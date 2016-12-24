#include "Common.h"

#include "ImageObject.h"
#include "ImageModule.h"


ImageModule::ImageModule() : PythonModule("image")
{
}
ImageModule::~ImageModule()
{
}
void ImageModule::init_module()
{
    PythonModule::init_module();

    add_type("Image", ImageObject::static_class());
}
