#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "Qt/QtVisWindow.h"
#include "VisPythonModule.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(VisPythonModule, show, ImageObject*);

VisPythonModule::VisPythonModule()
{
}
VisPythonModule::~VisPythonModule()
{
}
void VisPythonModule::post_init()
{
    MODULE_ADD_PYTHON_FUNCTION(VisPythonModule, show, "show(img)");
}
void VisPythonModule::show(ImageObject* obj)
{
    QtVisWindow* win = PlutoCore::instance().window_manager()->create_window<QtVisWindow>();
    QMetaObject::invokeMethod(win, "show", Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(win, "set_image", Qt::BlockingQueuedConnection, Q_ARG(ImageObject*, obj));
}
const char* VisPythonModule::name()
{
    return "vis";
}
