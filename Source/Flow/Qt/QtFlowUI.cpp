#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>

#include "FlowNode.h"

#include "Nodes/QtConstantNode.h"
#include "Nodes/QtGraphInputNode.h"
#include "Nodes/QtGraphOutputNode.h"
#include "Nodes/QtPrintNode.h"
#include "Nodes/QtSinglePinNode.h"
#include "Nodes/QtVariableNode.h"
#include "Nodes/QtTextViewNode.h"

#include "QtFlowNode.h"
#include "QtFlowUI.h"
#include "QtFlowWindow.h"

QtFlowUI::QtFlowUI()
{
    // Default factory
    install_ui_node_factory<QtFlowNode>("");
    
    install_ui_node_factory<QtConstantNode>("constant_node");
    install_ui_node_factory<QtGraphInputNode>("graph_input");
    install_ui_node_factory<QtGraphOutputNode>("graph_output");
    install_ui_node_factory<QtPrintNode>("print_node");
    install_ui_node_factory<QtSinglePinNode>("single_pin_node");
    install_ui_node_factory<QtVariableNode>("variable_node");
    install_ui_node_factory<QtTextViewNode>("text_view_node");
}
QtFlowUI::~QtFlowUI()
{
    for (auto w : _windows)
    {
        if (w->isVisible())
            w->close();
        delete w;
    }
    _windows.clear();

    for (auto f : _node_factories)
    {
        delete f.second;
    }
    _node_factories.clear();
}

QtFlowNode* QtFlowUI::create_ui_node(FlowNode* node)
{
    auto it = _node_factories.find(node->ui_class());
    if (it == _node_factories.end())
    {
        assert(false); // TODO:
        return nullptr;
    }

    QtFlowNode* n = it->second->create(node);
    n->setup();
    return n;
}

void QtFlowUI::install_ui_node_factory(const std::string& ui_class, UiNodeFactory* factory)
{
    assert(_node_factories.find(ui_class) == _node_factories.end());
    _node_factories[ui_class] = factory;
}

QtFlowWindow* QtFlowUI::create_window()
{
    QtFlowWindow* window = new QtFlowWindow();
    _windows.push_back(window);
    connect(this, SIGNAL(node_template_added(FlowNode*)), window, SIGNAL(node_template_added(FlowNode*)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(node_template_removed(FlowNode*)), window, SIGNAL(node_template_removed(FlowNode*)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(node_template_reloaded(FlowNode*)), window, SIGNAL(node_template_reloaded(FlowNode*)), Qt::BlockingQueuedConnection);
    return window;
}
