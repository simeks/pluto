#ifndef __FLOW_GRAPH_OUTPUT_NODE_H__
#define __FLOW_GRAPH_OUTPUT_NODE_H__

#include "FlowNode.h"

class GraphOutputNode : public FlowNode
{
    DECLARE_OBJECT(GraphOutputNode, FlowNode);

public:
    DECLARE_OBJECT_CONSTRUCTOR(GraphOutputNode);
    ~GraphOutputNode();

    void object_init();
    
    void run(FlowContext* ctx) OVERRIDE;

    /// Output name
    const char* name();
};


#endif // __FLOW_GRAPH_OUTPUT_NODE_H__
