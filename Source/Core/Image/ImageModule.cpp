#include "Common.h"

#include "ImageObject.h"
#include "ImageModule.h"


ImageModule::ImageModule() : PythonModule("image")
{
}
ImageModule::~ImageModule()
{
}
void ImageModule::create()
{
    PythonModule::create();

    add_type("Image", ImageObject::python_type());
}
