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
#include "Qt/QtFlowUI.h"
#include "Qt/QtFlowWindow.h"

namespace
{
    void debug_output(FlowContext*)
    {
        std::cout << "Waawwiwaiwa\n";
    }
}

PLUTO_IMPLEMENT_MODULE(FlowModule);

FlowModule* FlowModule::s_instance = nullptr;

FlowModule::FlowModule() : _py_module(nullptr), _ui(nullptr)
{
    if (s_instance == nullptr)
        s_instance = this;
}
FlowModule::~FlowModule()
{
    if (s_instance == this)
        s_instance = nullptr;
}

void FlowModule::install()
{
    PythonClass::ready_all();

    _ui = new QtFlowUI();
    _py_module = new FlowPythonModule(_ui);
    _py_module->init_module();
    PlutoCore::instance().install_python_module(_py_module);

    FlowPinDef pins[] = {
        {"In", FlowPin::In, ""},
        {0, 0, 0}
    };
    FlowNodeDef nd = { "flow.DebugOutput", "DebugOutput", "", pins, debug_output, "" };

    install_node_template(nd);
}
void FlowModule::uninstall()
{
    for (auto n : _node_templates)
    {
        n->release();
    }
    _node_templates.clear();

    delete _py_module;
    _py_module = nullptr;

    delete _ui;
    _ui = nullptr;
}
void FlowModule::install_node_template(FlowNode* node)
{
    _node_templates.push_back(node);
    emit _ui->node_template_added(node);
}
void FlowModule::install_node_template(const FlowNodeDef& def)
{
    FlowNode* node = object_new<FlowNode>(def);
    node->set_attribute("node_class", def.class_name);
    node->set_attribute("title", def.title);
    node->set_attribute("category", def.category);
    node->set_attribute("doc", def.doc);

    install_node_template(node);
}
FlowNode* FlowModule::node_template(const char* node_class) const
{
    for (auto n : _node_templates)
    {
        if (strcmp(n->node_class(), node_class) == 0)
            return n;
    }
    return nullptr;
}
const std::vector<FlowNode*>& FlowModule::node_templates() const
{
    return _node_templates;
}
FlowModule& FlowModule::instance()
{
    return *s_instance;
}

