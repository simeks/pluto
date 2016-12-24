#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
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


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, open, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, window);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowPythonModule, install_node_template, FlowNode*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, node_template, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, create_node, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, node_templates);

FlowPythonModule::FlowPythonModule(QtFlowUI* ui) : PythonModule("flow_api"), _ui(ui)
{
}
FlowPythonModule::~FlowPythonModule()
{
}
void FlowPythonModule::init_module()
{
    PythonModule::init_module();

    add_type("Context", FlowContext::static_class());
    add_type("Graph", FlowGraph::static_class());
    add_type("Node", FlowNode::static_class());
    add_type("Pin", FlowPin::static_class());

    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, open, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, window, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, install_node_template, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, node_template, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, create_node, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, node_templates, "");
}
FlowWindow* FlowPythonModule::open(const char* file)
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(_ui, "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    FlowWindow* winobj = object_new<FlowWindow>(win);
    if (file)
        winobj->load(file);
    return winobj;
}
FlowWindow* FlowPythonModule::window()
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(_ui, "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    FlowWindow* winobj = object_new<FlowWindow>(win);
    return winobj;
}
void FlowPythonModule::install_node_template(FlowNode* node)
{
    if (node->title() == nullptr)
        PYTHON_ERROR(AttributeError, "'title' not set");
    if (node->category() == nullptr)
        PYTHON_ERROR(AttributeError, "'category' not set");
    if (node->node_class() == nullptr)
        PYTHON_ERROR(AttributeError, "'node_class' not set");

    node->addref();
    FlowModule::instance().install_node_template(node);
}
FlowNode* FlowPythonModule::node_template(const char* node_class) const
{
    FlowNode* r = FlowModule::instance().node_template(node_class);
    if (!r)
        PYTHON_ERROR_R(KeyError, "no node of given class found", nullptr);

    r->addref();
    return r;
}
FlowNode* FlowPythonModule::create_node(const char* node_class) const
{
    FlowNode* n = node_template(node_class);
    if (n)
        return object_clone(n);
    return nullptr;
}

PyObject* FlowPythonModule::node_templates() const
{
    const std::vector<FlowNode*>& tpls = FlowModule::instance().node_templates();
    PyObject* l = PyList_New(tpls.size());
    for (int i = 0; i < tpls.size(); ++i)
    {
        PyList_SetItem(l, i, python_convert::to_python(tpls[i]->node_class()));
    }
    return l;
}