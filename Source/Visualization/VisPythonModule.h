#ifndef __VIS_PYTHON_MODULE_H__
#define __VIS_PYTHON_MODULE_H__

#include <Core/Python/PythonModule.h>

class ImageObject;
class VisPythonModule : public PythonModuleHelper<VisPythonModule>
{
public:
    VisPythonModule();
    ~VisPythonModule();

    void post_init() OVERRIDE;

    void show(ImageObject* obj);

    static const char* name();

};


#endif // __VIS_PYTHON_MODULE_H__
