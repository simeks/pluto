#include "Common.h"

#include "ImageObject.h"
#include "ImageModule.h"


ImageModule::ImageModule()
{
}
ImageModule::~ImageModule()
{
}
void ImageModule::post_init()
{
    add_type("Image", ImageObject::static_class());
}
const char* ImageModule::name()
{
    return "image";
}
