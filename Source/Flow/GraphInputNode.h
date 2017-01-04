#ifndef __FLOW_GRAPH_INPUT_NODE_H__
#define __FLOW_GRAPH_INPUT_NODE_H__

#include "FlowNode.h"

class GraphInputNode : public FlowNode
{
    DECLARE_OBJECT(GraphInputNode, FlowNode);

public:
    DECLARE_OBJECT_CONSTRUCTOR(GraphInputNode);
    ~GraphInputNode();

    void object_init();
    
    void run(FlowContext* ctx) OVERRIDE;

    /// Input name
    const char* name();
};


#endif // __FLOW_GRAPH_INPUT_NODE_H__
