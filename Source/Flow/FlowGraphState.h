#ifndef __FLOW_GRAPH_STATE_H__
#define __FLOW_GRAPH_STATE_H__

#include <Core/Python/Object.h>

class FlowGraph;
class FlowNode;
class FlowPin;

struct FlowGraphState
{
    FlowGraphState(FlowGraph* graph);
    ~FlowGraphState();

    void reset();

    void node_added(const Guid& node);
    void node_removed(const Guid& node);
    void node_changed(const Guid& node);
    void node_completed(const Guid& node);

    void build_execution_list();

    void find_dependents(const Guid& node_id, std::set<Guid>& dependents);

    FlowGraph* graph;
    std::map<Guid, bool> node_dirty_flags;
    FlowNode* current_node;

    std::map<FlowPin*, python::Object> pins;
    std::vector<FlowNode*> execution_list;

    std::map<std::string, python::Object> inputs;
    std::map<std::string, python::Object> outputs;
};

#endif // __FLOW_GRAPH_STATE_H__
