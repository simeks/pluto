#ifndef __FLOW_GRAPH_INPUT_NODE_H__
#define __FLOW_GRAPH_INPUT_NODE_H__

#include "FlowNode.h"

class GraphInputNode : public FlowNode
{
    PLUTO_OBJECT(GraphInputNode, FlowNode);

public:
    GraphInputNode();
    ~GraphInputNode();

    void run(FlowContext* ctx) OVERRIDE;

    /// Input name
    const char* name();
};


#endif // __FLOW_GRAPH_INPUT_NODE_H__
