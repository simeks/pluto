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
#include "RunGraphNode.h"

namespace
{
    void debug_output(FlowContext* ctx)
    {
        PYTHON_STDOUT("[Debug] ");
        PyObject* obj = ctx->read_pin("In");
        if (!obj)
        {
            PYTHON_STDOUT("NULL\n");
            return;
        }

        if (Object::static_class()->check_type(obj))
        {
            Object* o = python_convert::from_python<Object*>(obj);
            PYTHON_STDOUT("Object, class=%s", o->get_class()->name());
        }
        else
        {
            PYTHON_STDOUT("PyObject, type=%s, ", obj->ob_type->tp_name);
            PYTHON_STDOUT("%s", PyUnicode_AsUTF8(PyObject_Str(obj)));
        }
        PYTHON_STDOUT("\n");
    }
}

PLUTO_IMPLEMENT_MODULE(FlowModule);

FlowModule* FlowModule::s_instance = nullptr;

FlowModule::FlowModule() : _ui(nullptr)
{
    if (s_instance == nullptr)
        s_instance = this;
}
FlowModule::~FlowModule()
{
    delete _ui;
    _ui = nullptr;

    if (s_instance == this)
        s_instance = nullptr;
}

void FlowModule::install()
{
    _ui = new QtFlowUI();
    FlowPythonModule::create(_ui);
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
    _node_templates.push_back(object_new<GraphInputNode>());
    _node_templates.push_back(object_new<GraphOutputNode>());

    FlowPinDef pins[] = {
        { "In", FlowPin::In, "" },
        { 0, 0, 0 }
    };
    FlowNodeDef nd = { "flow.DebugOutput", "DebugOutput", "Debug", pins, debug_output, "" };

    install_node_template(nd);
}
void FlowModule::install_node_template(FlowNode* node)
{
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
void FlowModule::install_node_template(const FlowNodeDef& def)
{
    FlowNode* node = object_new<FlowNode>(def);
    node->set_attribute("node_class", def.class_name);
    node->set_attribute("title", def.title);
    node->set_attribute("category", def.category);
    node->set_attribute("doc", def.doc);

    install_node_template(node);
    node->release();
}
FlowNode* FlowModule::node_template(const char* node_class) const
{
    for (auto& n : _node_templates)
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

