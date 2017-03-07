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
    // We don't need to expose the class but we have to make sure that it is initialized.
    // python_class() will hold the reference for the duration of the program.
    FlowWindow::python_class();

    module.def("FlowContext", FlowContext::static_class());
    module.def("FlowGraph", FlowGraph::static_class());
    module.def("FlowNode", FlowNode::static_class());
    module.def("UiFlowNode", UiFlowNode::static_class());
    module.def("FlowPin", FlowPin::static_class());
    module.def("ArrayFlowPin", ArrayFlowPin::static_class());
    module.def("FlowProperty", FlowProperty::static_class());
    module.def("BoolProperty", BoolProperty::static_class());
    module.def("IntProperty", IntProperty::static_class());
    module.def("FloatProperty", FloatProperty::static_class());
    module.def("EnumProperty", EnumProperty::static_class());
    module.def("FileProperty", FileProperty::static_class());
    module.def("StringProperty", StringProperty::static_class());

    module.def("open", &flow::open, "open(file)");
    module.def("window", &flow::window, "window()");

    module.def("load", &flow::load, "load(file)");
    module.def("save", &flow::save, "save(file, graph)");
    module.def_varargs_keywords("run", &flow::run, "run(graph, **kwargs)\n"
                                                        "--\n"
                                                        "Runs a graph");

    module.def("install_node_template", &FlowModule::instance(), &FlowModule::install_node_template, "install_node_template(node)");
    module.def("node_templates", &flow::node_templates, "node_templates()");
    module.def("create_node", &flow::create_node, "create_node(cls)");
    
    module.def("add_graph_path", &FlowModule::instance(), &FlowModule::add_graph_path, "add_graph_path(path)");

}
std::unique_ptr<FlowWindow> flow::open(const char* file)
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(FlowModule::instance().ui(), "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    std::unique_ptr<FlowWindow> winobj = std::make_unique<FlowWindow>(win);
    if (file)
        winobj->load(file);
    return winobj;
}
std::unique_ptr<FlowWindow> flow::window()
{
    QtFlowWindow* win = nullptr;
    QMetaObject::invokeMethod(FlowModule::instance().ui(), "create_window", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QtFlowWindow*, win));
    QMetaObject::invokeMethod(win, "show");

    return std::make_unique<FlowWindow>(win);
}
FlowGraph* flow::load(const char* file)
{
    JsonObject obj;
    JsonReader reader;
    if (!reader.read_file(file, obj))
        PYTHON_ERROR(PyExc_IOError, reader.error_message().c_str());

    FlowGraph* graph = flow_graph::load(obj);
    if (!graph)
        PYTHON_ERROR(PyExc_IOError, "Failed to parse graph file");

    return graph;
}
void flow::save(const char* file, FlowGraph* graph)
{
    JsonObject obj;
    flow_graph::save(graph, obj);

    JsonWriter writer;
    if (!writer.write_file(obj, file, true))
        PYTHON_ERROR(PyExc_IOError, "Failed to write graph file");

}
Dict flow::run(const Tuple& args, const Dict& kw)
{
    if (args.size() < 1)
        PYTHON_ERROR(PyExc_ValueError, "Expected at least 1 argument");

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
            PYTHON_ERROR(PyExc_IOError, reader.error_message().c_str());

        graph = flow_graph::load(obj);
    }
    else
        PYTHON_ERROR(PyExc_ValueError, "Invalid argument, expected graph or path to graph file");

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
        python::Object obj = context->output(it.first.c_str());
        if (!obj.is_none())
        {
            ret.set(it.first.c_str(), obj);
        }
        else
        {
            ret.set(it.first.c_str(), python::None());
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
