#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "Qt/QtVisWindow.h"
#include "VisPythonModule.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(VisPythonModule, show, Image);

VisPythonModule::VisPythonModule() : _current_window(nullptr)
{
}
VisPythonModule::~VisPythonModule()
{
}
void VisPythonModule::post_init()
{
    MODULE_ADD_PYTHON_FUNCTION(VisPythonModule, show, "show(img)");
}
void VisPythonModule::show(const Image& img)
{
    if (!_current_window)
    {
        _current_window = PlutoCore::instance().window_manager()->create_window<QtVisWindow>();
    }
    QMetaObject::invokeMethod(_current_window, "show", Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(_current_window, "set_image", Qt::BlockingQueuedConnection, Q_ARG(const Image&, img));
}
const char* VisPythonModule::name()
{
    return "vis";
}
