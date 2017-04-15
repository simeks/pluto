#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowGraphState.h"
#include "FlowPin.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"

FlowGraphState::FlowGraphState(FlowGraph* g) : 
    graph(g),
    current_node(nullptr)
{
    graph->addref();

    for (auto& n : graph->nodes())
    {
        node_dirty_flags[n.second->node_id()] = true;

        if (n.second->is_a(GraphInputNode::static_class()))
        {
            inputs[n.second->attribute<const char*>("name")] = python::None();
        }
        else if (n.second->is_a(GraphOutputNode::static_class()))
        {
            outputs[n.second->attribute<const char*>("name")] = python::None();
        }
    }
}
FlowGraphState::~FlowGraphState()
{
    graph->release();
}
void FlowGraphState::reset()
{
    for (auto& n : graph->nodes())
    {
        node_dirty_flags[n.second->node_id()] = true;
    }
}
void FlowGraphState::node_added(const Guid& node)
{
    node_dirty_flags[node] = true;
}
void FlowGraphState::node_removed(const Guid& node)
{
    auto it = node_dirty_flags.find(node);
    if (it != node_dirty_flags.end())
        node_dirty_flags.erase(it);
}
void FlowGraphState::node_changed(const Guid& node)
{
    node_dirty_flags[node] = true;

    // Update all dependents
    std::set<Guid> dependents;
    find_dependents(node, dependents);

    for (auto& n : dependents)
    {
        node_dirty_flags[n] = true;
    }
}
void FlowGraphState::node_completed(const Guid& node)
{
    node_dirty_flags[node] = false;
}
void FlowGraphState::build_execution_list()
{
    execution_list.clear();

    std::vector<FlowNode*> terminal_nodes;
    std::map<FlowNode*, int> num_incoming_edges;
    int total_incoming_edges_left = 0;

    for (auto& node_it : graph->nodes())
    {
        FlowNode* node = node_it.second;

        // Skip clean nodes
        if (node_dirty_flags[node->node_id()] == false)
            continue;

        int incoming_edges = 0;
        for (auto& pin : node->pins())
        {
            if (pin->pin_type() == FlowPin::In && !pin->links().empty())
            {
                FlowNode* src = pin->links()[0]->owner();
                // Only count edge if source node is dirty
                if (node_dirty_flags[src->node_id()] == false)
                    continue;

                ++incoming_edges;
            }
        }
        num_incoming_edges[node] = incoming_edges;
        total_incoming_edges_left += incoming_edges;

        if (incoming_edges == 0)
        {
            terminal_nodes.push_back(node);
        }
    }

    while (!terminal_nodes.empty())
    {
        FlowNode* node = terminal_nodes.back();
        terminal_nodes.pop_back();

        execution_list.push_back(node);

        for (auto& pin : node->pins())
        {
            if (pin->pin_type() == FlowPin::Out)
            {
                for (auto& link : pin->links())
                {
                    auto incoming_edges = num_incoming_edges.find(link->owner());
                    if (incoming_edges != num_incoming_edges.end())
                    {
                        int& num = incoming_edges->second;
                        --num;
                        --total_incoming_edges_left;

                        if (num <= 0)
                        {
                            terminal_nodes.push_back(link->owner());
                        }
                    }
                }
            }
        }
    }

    if (total_incoming_edges_left)
    {
        execution_list.clear();
        PYTHON_ERROR(PyExc_RuntimeError, "Failed to compile graph: Cycles detected.");
    }
}
void FlowGraphState::find_dependents(const Guid& node_id, std::set<Guid>& dependents)
{
    FlowNode* node = graph->node(node_id);
    assert(node);
    for (auto p : node->pins())
    {
        if (p->pin_type() == FlowPin::Out)
        {
            for (auto& p2 : p->links())
            {
                dependents.emplace(p2->owner()->node_id());
            }
        }
    }
}


