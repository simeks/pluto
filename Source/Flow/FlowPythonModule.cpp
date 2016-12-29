#include <Core/Common.h>
#include <Core/Json/Json.h>
#include <Core/Json/JsonObject.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "FlowPythonModule.h"
#include "FlowWindow.h"
#include "Qt/QtFlowUI.h"
#include "Qt/QtFlowWindow.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, open, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, window);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, load, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowPythonModule, save, const char*, FlowGraph*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowPythonModule, install_node_template, FlowNode*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, node_template, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, create_node, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, node_templates);


FlowPythonModule::FlowPythonModule(QtFlowUI* ui) : _ui(ui)
{
}
FlowPythonModule::~FlowPythonModule()
{
}
void FlowPythonModule::post_init()
{
    add_type("Context", FlowContext::static_class());
    add_type("Graph", FlowGraph::static_class());
    add_type("Node", FlowNode::static_class());
    add_type("Pin", FlowPin::static_class());
    add_type("Property", FlowProperty::static_class());

    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, open, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, window, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, load, "");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, save, "");
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
FlowGraph* FlowPythonModule::load(const char* file)
{
    JsonObject obj;
    JsonReader reader;
    if (!reader.read_file(file, obj))
        PYTHON_ERROR_R(IOError, nullptr, reader.error_message().c_str());

    FlowGraph* graph = flow_graph::load(obj);
    if (!graph)
        PYTHON_ERROR_R(IOError, nullptr, "Failed to parse graph file");

    return graph;
}
void FlowPythonModule::save(const char* file, FlowGraph* graph)
{
    JsonObject obj;
    flow_graph::save(graph, obj);

    JsonWriter writer;
    if (!writer.write_file(obj, file, true))
        PYTHON_ERROR(IOError, "Failed to write graph file");

}
void FlowPythonModule::install_node_template(FlowNode* node)
{
    if (node->title() == nullptr)
        PYTHON_ERROR(AttributeError, "'title' not set");
    if (node->category() == nullptr)
        PYTHON_ERROR(AttributeError, "'category' not set");
    if (node->node_class() == nullptr)
        PYTHON_ERROR(AttributeError, "'node_class' not set");

    FlowModule::instance().install_node_template(node);
}
FlowNode* FlowPythonModule::node_template(const char* node_class) const
{
    FlowNode* r = FlowModule::instance().node_template(node_class);
    if (!r)
        PYTHON_ERROR_R(KeyError, nullptr, "no node of given class found");

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
const char* FlowPythonModule::name()
{
    return "flow_api";
}
