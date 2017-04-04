#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "GraphInputNode.h"

PYTHON_OBJECT_IMPL(GraphInputNode, "GraphInputNode")
{
    cls;
}

GraphInputNode::GraphInputNode() :
    FlowNode("flow.GraphInputNode", "Input", "Flow/Graph")
{
    add_pin("Out", FlowPin::Out);
    //set_attribute("doc", "Graph input node");

    set_ui_class("graph_input");

    add_property(python::make_object<FlowProperty>(python::to_python("")));
}
GraphInputNode::~GraphInputNode()
{
}
void GraphInputNode::run(FlowContext* ctx)
{
    ctx->write_pin("Out", ctx->input(name()));
}
const char* GraphInputNode::name()
{
    return attribute<const char*>("name");
}
