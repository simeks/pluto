#ifndef __FLOW_RUN_GRAPH_NODE_H__
#define __FLOW_RUN_GRAPH_NODE_H__

#include "FlowNode.h"

/// For running a graph within a graph
class RunGraphNode : public FlowNode
{
    DECLARE_OBJECT(RunGraphNode, FlowNode);

public:
    DECLARE_OBJECT_CONSTRUCTOR(RunGraphNode);
    ~RunGraphNode();

    void object_init();

    bool load_graph(const char* class_name, const char* file);
    void set_graph(const char* class_name, FlowGraph* graph);

    void run(FlowContext* ctx) OVERRIDE;

    bool valid() const;

private:
    void create_pins();

    FlowGraph* _graph;
    FlowContext* _context;
};


#endif // __FLOW_RUN_GRAPH_NODE_H__
