#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>

#include "QtFlowUI.h"
#include "QtFlowWindow.h"

QtFlowUI::QtFlowUI()
{
}
QtFlowUI::~QtFlowUI()
{
    for (auto w : _windows)
    {
        w->close();
        delete w;
    }
    _windows.clear();
}
QtFlowWindow* QtFlowUI::create_window()
{
    QtFlowWindow* window = new QtFlowWindow();
    _windows.push_back(window);
    connect(this, SIGNAL(node_template_added(FlowNode*)), window, SIGNAL(node_template_added(FlowNode*)));
    connect(this, SIGNAL(node_template_removed(FlowNode*)), window, SIGNAL(node_template_removed(FlowNode*)));
    return window;
}
