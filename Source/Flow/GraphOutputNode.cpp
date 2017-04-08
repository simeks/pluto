#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "GraphOutputNode.h"

PLUTO_OBJECT_IMPL(GraphOutputNode, "GraphOutputNode")
{
    cls;
}

GraphOutputNode::GraphOutputNode() :
    FlowNode("flow.GraphOutputNode", "Output", "Flow/Graph")
{
    add_pin("In", FlowPin::In);

    auto property_name = make_object<FlowProperty>(python::to_python(""));
    property_name->set_name("name");
    add_property(property_name);

    set_ui_class("graph_output");
}
GraphOutputNode::~GraphOutputNode()
{
}

void GraphOutputNode::run(FlowContext* ctx)
{
    ctx->set_output(name(), ctx->read_pin("In"));
}
const char* GraphOutputNode::name() const
{
    return property_value<const char*>("name");
}
