#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "GraphInputNode.h"

OBJECT_INIT_TYPE_FN(GraphInputNode)
{
    OBJECT_PYTHON_NO_METHODS();
}


IMPLEMENT_OBJECT(GraphInputNode, "GraphInputNode", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(GraphInputNode, FlowNode);

GraphInputNode::~GraphInputNode()
{
}

void GraphInputNode::object_init()
{
    set_attribute("node_class", "flow.GraphInput");
    set_attribute("title", "Input");
    set_attribute("category", "Flow/Graph");
    set_attribute("doc", "Graph input node");

    add_pin("Out", FlowPin::Out);
    add_property(object_new<FlowProperty>("name", ""));
}
void GraphInputNode::run(FlowContext* ctx)
{
    ctx->write_pin("Out", ctx->graph_input(name()));
}
const char* GraphInputNode::name()
{
    return attribute<const char*>("name");
}
