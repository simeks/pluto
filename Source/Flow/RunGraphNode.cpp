#include <Core/Common.h>
#include <Core/Platform/FilePath.h>
#include <Core/Json/Json.h>
#include <Core/Json/JsonObject.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"
#include "RunGraphNode.h"

OBJECT_INIT_TYPE_FN(RunGraphNode)
{
    OBJECT_PYTHON_NO_METHODS();
}


IMPLEMENT_OBJECT(RunGraphNode, "RunGraphNode", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(RunGraphNode, FlowNode);

RunGraphNode::~RunGraphNode()
{
    if (_graph)
        _graph->release();
}

void RunGraphNode::object_init()
{
    _graph = nullptr;
}
bool RunGraphNode::load_graph(const char* class_name, const char* file)
{
    JsonObject obj;
    JsonReader reader;
    if (!reader.read_file(file, obj))
    {
        std::cerr << reader.error_message() << std::endl; // TODO:
        return false;
    }

    _graph = flow_graph::load(obj);
    if (!_graph)
        return false;

    FilePath path(file);
    path = path.filename();

    set_attribute("node_class", class_name);

    path.trim_extension();
    set_attribute("title", path.get());

    set_attribute("category", "Graphs");
    set_attribute("doc", "");

    create_pins();

    return true;
}
void RunGraphNode::set_graph(const char* class_name, FlowGraph* graph)
{
    set_attribute("node_class", class_name);
    set_attribute("title", class_name);

    set_attribute("category", "Graphs");
    set_attribute("doc", "");

    _graph = graph;
    _graph->addref();

    create_pins();
}
void RunGraphNode::create_pins()
{
    if (!_graph)
        return;

    for (auto it : _graph->nodes())
    {
        if (it.second->is_a(GraphInputNode::static_class()))
        {
            add_pin(object_cast<GraphInputNode>(it.second)->name(), FlowPin::In);
        }
        if (it.second->is_a(GraphOutputNode::static_class()))
        {
            add_pin(object_cast<GraphOutputNode>(it.second)->name(), FlowPin::Out);
        }
    }

}
void RunGraphNode::run(FlowContext* ctx)
{
    if (!_graph)
        return;

    FlowContext* sub_ctx = ctx->create_child_context(_graph);
    for (auto p : pins())
    {
        if (p->pin_type() == FlowPin::In)
        {
            sub_ctx->set_input(p->name(), ctx->read_pin(p->name()));
        }
    }

    sub_ctx->run();

    for (auto p : pins())
    {
        if (p->pin_type() == FlowPin::Out)
        {
            ctx->write_pin(p->name(), sub_ctx->output(p->name()));
        }
    }
}
bool RunGraphNode::valid() const
{
    return _graph != nullptr;
}