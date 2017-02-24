#include <Core/Common.h>
#include <Core/Json/Json.h>
#include <Core/Json/JsonObject.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
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

namespace py = python;

PYTHON_MODULE(flow_api)
{
    py::def(module, "FlowContext", FlowContext::static_class());
    py::def(module, "FlowGraph", FlowGraph::static_class());
    py::def(module, "FlowNode", FlowNode::static_class());
    py::def(module, "UiFlowNode", UiFlowNode::static_class());
    py::def(module, "FlowPin", FlowPin::static_class());
    py::def(module, "ArrayFlowPin", ArrayFlowPin::static_class());
    py::def(module, "FlowProperty", FlowProperty::static_class());
    py::def(module, "BoolProperty", BoolProperty::static_class());
    py::def(module, "IntProperty", IntProperty::static_class());
    py::def(module, "FloatProperty", FloatProperty::static_class());
    py::def(module, "EnumProperty", EnumProperty::static_class());
    py::def(module, "FileProperty", FileProperty::static_class());
    py::def(module, "StringProperty", StringProperty::static_class());

    py::def(module, "open", &flow::open, "open(file)");
    py::def(module, "window", &flow::window, "window()");

    py::def(module, "load", &flow::load, "load(file)");
    py::def(module, "save", &flow::save, "save(file, graph)");
    py::def_varargs_keywords(module, "run", &flow::run, "run(graph, **kwargs)\n"
                                                        "--\n"
                                                        "Runs a graph");

    py::def(module, "install_node_template", &FlowModule::instance(), &FlowModule::install_node_template, "install_node_template(node)");
    py::def(module, "node_templates", &flow::node_templates, "node_templates()");
    py::def(module, "create_node", &flow::create_node, "create_node(cls)");
    
    py::def(module, "add_graph_path", &FlowModule::instance(), &FlowModule::add_graph_path, "add_graph_path(path)");

}

FlowWindow* flow::open(const char* file)
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(FlowModule::instance().ui(), "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    FlowWindow* winobj = object_new<FlowWindow>(win);
    if (file)
        winobj->load(file);
    return winobj;
}
FlowWindow* flow::window()
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(FlowModule::instance().ui(), "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    FlowWindow* winobj = object_new<FlowWindow>(win);
    return winobj;
}
FlowGraph* flow::load(const char* file)
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
void flow::save(const char* file, FlowGraph* graph)
{
    JsonObject obj;
    flow_graph::save(graph, obj);

    JsonWriter writer;
    if (!writer.write_file(obj, file, true))
        PYTHON_ERROR(IOError, "Failed to write graph file");

}
Dict flow::run(const Tuple& args, const Dict& kw)
{
    if (args.size() < 1)
        PYTHON_ERROR_R(ValueError, Dict(), "Expected at least 1 argument");

    FlowGraph* graph = nullptr;
    if (FlowGraph::static_class()->check_type(args.get(0).ptr()))
        graph = python::from_python<FlowGraph*>(args.get(0));
    else if (PyUnicode_Check(args.get(0).ptr()))
    {
        // First argument was a string so we assume its a path to a graph file
        const char* file = PyUnicode_AsUTF8(args.get(0).ptr());

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
            context->set_input(it.first.c_str(), python::incref(kw.get(it.first.c_str())));
        }
    }

    if (!context->run())
        return Dict();

    Dict ret;
    for (auto& it : context->outputs())
    {
        python::Object obj = context->output(it.first.c_str());
        if (!obj.is_none())
        {
            ret.set(it.first.c_str(), python::incref(obj));
        }
        else
        {
            ret.set(it.first.c_str(), python::incref(Py_None));
        }
    }
    return ret;
}
FlowNode* flow::create_node(const char* node_class)
{
    FlowNode* n = FlowModule::instance().node_template(node_class);
    if (n)
        return object_clone(n);
    return nullptr;
}
Tuple flow::node_templates()
{
    const std::vector<FlowNode*>& tpls = FlowModule::instance().node_templates();
    Tuple l(tpls.size());
    for (int i = 0; i < tpls.size(); ++i)
    {
        l.set(i, python::to_python(tpls[i]->node_class()));
    }
    return l;
}
void flow::install_python_module()
{
    PYTHON_MODULE_INSTALL(flow_api);
}
