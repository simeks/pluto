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
#include "Qt/QtGraphFileLoader.h"
#include "UiFlowNode.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, open, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, window);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, load, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowPythonModule, save, const char*, FlowGraph*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowPythonModule, install_node_template, FlowNode*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, node_template, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowPythonModule, create_node, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowPythonModule, add_graph_path, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowPythonModule, node_templates);
PYTHON_FUNCTION_WRAPPER_CLASS_TUPLE_DICT_RETURN(FlowPythonModule, run);


FlowPythonModule::FlowPythonModule(QtFlowUI* ui) : _ui(ui)
{
}
FlowPythonModule::~FlowPythonModule()
{
    for (auto& l : _loaders)
        delete l;
    _loaders.clear();
}
void FlowPythonModule::post_init()
{
    add_type("FlowContext", FlowContext::static_class());
    add_type("FlowGraph", FlowGraph::static_class());
    add_type("FlowNode", FlowNode::static_class());
    add_type("UiFlowNode", UiFlowNode::static_class());
    add_type("FlowPin", FlowPin::static_class());
    add_type("ArrayFlowPin", ArrayFlowPin::static_class());
    add_type("FlowProperty", FlowProperty::static_class());
    add_type("BoolProperty", BoolProperty::static_class());
    add_type("IntProperty", IntProperty::static_class());
    add_type("FloatProperty", FloatProperty::static_class());
    add_type("EnumProperty", EnumProperty::static_class());
    add_type("FileProperty", FileProperty::static_class());
    add_type("StringProperty", StringProperty::static_class());

    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, open, "open(file)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, window, "window()");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, load, "load(file)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, save, "save(file, graph)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, install_node_template, "install_node_template(node)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, node_template, "node_template(cls)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, create_node, "create_node(cls)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, add_graph_path, "add_graph_path(path)");
    MODULE_ADD_PYTHON_FUNCTION(FlowPythonModule, node_templates, "node_templates()");
    MODULE_ADD_PYTHON_KEYWORD_FUNCTION(FlowPythonModule, run, "run(graph, **kwargs)\n"
        "--\n"
        "Runs a graph");
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
void FlowPythonModule::add_graph_path(const char* path)
{
    for (auto& l : _loaders)
    {
        if (strcmp(path, l->path()) == 0)
            return;
    }

    _loaders.push_back(new GraphFileLoader(path, this));
}
Tuple FlowPythonModule::node_templates() const
{
    const std::vector<FlowNode*>& tpls = FlowModule::instance().node_templates();
    Tuple l(tpls.size());
    for (int i = 0; i < tpls.size(); ++i)
    {
        l.set(i, python_convert::to_python(tpls[i]->node_class()));
    }
    return l;
}
Dict FlowPythonModule::run(const Tuple& args, const Dict& kw)
{
    if (args.size() < 1)
        PYTHON_ERROR_R(ValueError, Dict(), "Expected at least 1 argument");

    FlowGraph* graph = nullptr;
    if (FlowGraph::static_class()->check_type(args.get(0)))
        graph = python_convert::from_python<FlowGraph*>(args.get(0));
    else if (PyUnicode_Check(args.get(0)))
    {
        // First argument was a string so we assume its a path to a graph file
        const char* file = PyUnicode_AsUTF8(args.get(0));

        JsonObject obj;
        JsonReader reader;
        if (!reader.read_file(file, obj))
            PYTHON_ERROR_R(IOError, Dict(), reader.error_message().c_str());

        graph = flow_graph::load(obj);
    }
    else
        PYTHON_ERROR_R(ValueError, Dict(), "Invalid argument, expected graph or path to graph file");

    FlowContext* context = object_new<FlowContext>(graph);

    if (kw.valid())
    {
        for (auto& it : context->inputs())
        {
            context->set_input(it.first.c_str(), kw.get(it.first.c_str()));
        }
    }

    if (!context->run())
        return Dict();

    Dict ret;
    for (auto& it : context->outputs())
    {
        PyObject* obj = context->output(it.first.c_str());
        if (obj)
        {
            Py_INCREF(obj);
            ret.set(it.first.c_str(), obj);
        }
        else
        {
            Py_INCREF(Py_None);
            ret.set(it.first.c_str(), Py_None);
        }
    }
    return ret;
}
const char* FlowPythonModule::name()
{
    return "flow_api";
}
