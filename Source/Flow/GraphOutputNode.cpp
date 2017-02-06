#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "GraphOutputNode.h"

OBJECT_INIT_TYPE_FN(GraphOutputNode)
{
    OBJECT_PYTHON_NO_METHODS();
}


IMPLEMENT_OBJECT(GraphOutputNode, "GraphOutputNode", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(GraphOutputNode, FlowNode);

GraphOutputNode::~GraphOutputNode()
{
}

void GraphOutputNode::object_init()
{
    set_attribute("node_class", "flow.GraphOutput");
    set_attribute("title", "Output");
    set_attribute("category", "Flow/Graph");
    set_attribute("doc", "Graph output node");

    set_attribute("ui_class", "graph_output");

    add_pin("In", FlowPin::In);
    add_property(object_new<StringProperty>("name", ""));
}
void GraphOutputNode::run(FlowContext* ctx)
{
    ctx->set_output(name(), ctx->read_pin("In"));
}
const char* GraphOutputNode::name()
{
    return attribute<const char*>("name");
}
