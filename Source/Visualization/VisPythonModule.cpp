#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
#include <Core/Qt/WindowManager.h>

#include <Flow/FlowModule.h>

#include "Qt/QtVisWindow.h"
#include "VisPythonModule.h"

PYTHON_MODULE(vis)
{
    PYTHON_MODULE_FUNCTION("show", &vis::show);
}

namespace
{
    QtVisWindow* _current_window = nullptr;
}

void vis::show(const Image& img)
{
    if (!_current_window)
    {
        _current_window = PlutoCore::instance().window_manager()->create_window<QtVisWindow>();
    }
    QMetaObject::invokeMethod(_current_window, "show", Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(_current_window, "set_image", Qt::BlockingQueuedConnection, Q_ARG(const Image&, img));
}
void vis::install_python_module()
{
    PYTHON_MODULE_INSTALL(vis);
}

