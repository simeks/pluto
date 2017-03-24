#ifndef __FLOW_GRAPH_RUNNER_H__
#define __FLOW_GRAPH_RUNNER_H__

class FlowGraph;
class FlowNode;
class FlowPin;

struct FlowGraphState
{
    struct Node
    {
        FlowNode* node;
        int ndeps;
        bool dirty;
    };

    FlowGraphState();
    ~FlowGraphState();

    FlowGraph* graph;
    std::vector<Node> nodes;
    FlowNode* current_node;

    std::map<FlowPin*, python::Object> pins;
    std::vector<FlowNode*> nodes_to_execute;

    std::map<std::string, python::Object> inputs;
    std::map<std::string, python::Object> outputs;
};

namespace flow_graph
{
    FlowGraphState* build_state(FlowGraph* graph);
    void release_state(FlowGraphState* state);



}


#endif // __FLOW_GRAPH_RUNNER_H__
