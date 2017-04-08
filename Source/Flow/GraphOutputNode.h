#ifndef __FLOW_GRAPH_OUTPUT_NODE_H__
#define __FLOW_GRAPH_OUTPUT_NODE_H__

#include "FlowNode.h"

class GraphOutputNode : public FlowNode
{
    PLUTO_OBJECT(GraphOutputNode, FlowNode);

public:
    GraphOutputNode();
    ~GraphOutputNode();

    void run(FlowContext* ctx) OVERRIDE;

    /// Output name
    const char* name() const;
};


#endif // __FLOW_GRAPH_OUTPUT_NODE_H__
