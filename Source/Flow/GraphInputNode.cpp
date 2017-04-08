#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "GraphInputNode.h"

PLUTO_OBJECT_IMPL(GraphInputNode, "GraphInputNode")
{
    cls;
}

GraphInputNode::GraphInputNode() :
    FlowNode("flow.GraphInputNode", "Input", "Flow/Graph")
{
    add_pin("Out", FlowPin::Out);

    auto property_name = make_object<FlowProperty>(python::to_python(""));
    property_name->set_name("name");
    add_property(property_name);

    set_ui_class("graph_input");
}
GraphInputNode::~GraphInputNode()
{
}
void GraphInputNode::run(FlowContext* ctx)
{
    ctx->write_pin("Out", ctx->input(name()));
}
const char* GraphInputNode::name() const
{
    return property_value<const char*>("name");
}
