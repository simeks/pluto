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

PYTHON_OBJECT_IMPL(RunGraphNode, "RunGraphNode")
{
    cls;
}

RunGraphNode::RunGraphNode() :
    _graph(nullptr),
    _context(nullptr)
{
}
RunGraphNode::~RunGraphNode()
{
    if (_graph)
        _graph->release();
    if (_context)
        _context->release();
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
            add_pin(python::object_cast<GraphInputNode>(it.second)->name(), FlowPin::In);
        }
        if (it.second->is_a(GraphOutputNode::static_class()))
        {
            add_pin(python::object_cast<GraphOutputNode>(it.second)->name(), FlowPin::Out);
        }
    }

}
void RunGraphNode::run(FlowContext* ctx)
{
    if (!_graph)
        return;

    if (!_context)
    {
        _context = ctx->create_child_context(_graph);

        for (auto p : pins())
        {
            if (p->pin_type() == FlowPin::In)
            {
                _context->set_input(p->name(), ctx->read_pin(p->name()));
            }
        }
    }

    if (!_context->run())
    {
        ctx->raise_error(_context->error());
        return;
    }

    for (auto p : pins())
    {
        if (p->pin_type() == FlowPin::Out)
        {
            ctx->write_pin(p->name(), _context->output(p->name()));
        }
    }

    _context->release();
    _context = nullptr;
}
bool RunGraphNode::valid() const
{
    return _graph != nullptr;
}
RunGraphNode::RunGraphNode(const RunGraphNode& other) : FlowNode(other)
{
    _context = nullptr;
    _graph = python::clone_object(other._graph);
}
