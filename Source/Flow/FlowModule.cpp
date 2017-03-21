#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "FlowPythonModule.h"
#include "FlowWindow.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"
#include "Qt/QtFlowUI.h"
#include "Qt/QtFlowWindow.h"
#include "Qt/QtGraphFileLoader.h"
#include "RunGraphNode.h"


PLUTO_IMPLEMENT_MODULE(FlowModule);

FlowModule* FlowModule::s_instance = nullptr;

FlowModule::FlowModule() : _ui(nullptr)
{
    if (s_instance == nullptr)
        s_instance = this;
}
FlowModule::~FlowModule()
{
    for (auto& l : _loaders)
        delete l;
    _loaders.clear();

    delete _ui;
    _ui = nullptr;

    if (s_instance == this)
        s_instance = nullptr;
}

void FlowModule::install()
{
    _ui = new QtFlowUI();
    flow::install_python_module();
}
void FlowModule::uninstall()
{
    for (auto& n : _node_templates)
    {
        n->release();
    }
    _node_templates.clear();
}
void FlowModule::init()
{
    _node_templates.push_back(python::make_object<GraphInputNode>());
    _node_templates.push_back(python::make_object<GraphOutputNode>());
}
void FlowModule::install_node_template(FlowNode* node)
{
    if (node->title() == nullptr)
        PYTHON_ERROR(PyExc_AttributeError, "'title' not set");
    if (node->category() == nullptr)
        PYTHON_ERROR(PyExc_AttributeError, "'category' not set");
    if (node->node_class() == nullptr)
        PYTHON_ERROR(PyExc_AttributeError, "'node_class' not set");

    bool reload = false;

    auto it = std::find_if(_node_templates.begin(), _node_templates.end(), [&](FlowNode* n) { return strcmp(n->node_class(), node->node_class()) == 0; });
    if (it != _node_templates.end())
    {
        reload = true;
        (*it)->release();
        _node_templates.erase(it);
    }

    node->addref();
    _node_templates.push_back(node);

    if (reload)
        emit _ui->node_template_reloaded(node);
    else
        emit _ui->node_template_added(node);
}
FlowNode* FlowModule::node_template(const char* node_class)
{
    for (auto& n : _node_templates)
    {
        if (strcmp(n->node_class(), node_class) == 0)
        {
            return n;
        }
    }

    PYTHON_ERROR(PyExc_KeyError, "no node of given class found");
}
const std::vector<FlowNode*>& FlowModule::node_templates() const
{
    return _node_templates;
}
void FlowModule::add_graph_path(const char* path)
{
    for (auto& l : _loaders)
    {
        if (strcmp(path, l->path()) == 0)
            return;
    }
    _loaders.push_back(new GraphFileLoader(path));
}
QtFlowUI* FlowModule::ui() const
{
    return _ui;
}
FlowModule& FlowModule::instance()
{
    return *s_instance;
}

