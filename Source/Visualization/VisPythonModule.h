#ifndef __VIS_PYTHON_MODULE_H__
#define __VIS_PYTHON_MODULE_H__

#include <Core/Python/PythonModule.h>

class Image;
class VisPythonModule : public PythonModuleHelper<VisPythonModule>
{
public:
    VisPythonModule();
    ~VisPythonModule();

    void post_init() OVERRIDE;

    void show(const Image& obj);

    static const char* name();

};


#endif // __VIS_PYTHON_MODULE_H__
