#ifndef __VIS_PYTHON_MODULE_H__
#define __VIS_PYTHON_MODULE_H__

#include <Core/Python/PythonModule.h>

class Image;
class QtVisWindow;
class VisPythonModule : public PythonModuleHelper<VisPythonModule>
{
public:
    VisPythonModule();
    ~VisPythonModule();

    void post_init() OVERRIDE;

    void show(const Image& obj);

    static const char* name();


private:
    QtVisWindow* _current_window;
};


#endif // __VIS_PYTHON_MODULE_H__
