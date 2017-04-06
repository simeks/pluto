#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowGraph;
class FlowNode;
class FlowPin;
class GraphNote;

class FLOW_API FlowGraph : public Object
{
    PLUTO_OBJECT(FlowGraph, Object);

public:
    FlowGraph();
    ~FlowGraph();

    void add_node(FlowNode* node);
    void remove_node(FlowNode* node);

    void clear();

    /// Tries to add a link from a to b
    /// Returns true if successful, false if not
    bool try_add_link(FlowPin* a, FlowPin* b); 
    void remove_link(FlowPin* a, FlowPin* b);

    /// Removes all links associated with pin
    void remove_links(FlowPin* pin);
    /// Removes all links associated with node
    void remove_links(FlowNode* node);

    /// Attempts to find the node with the specified id
    /// Returns nullptr if no node was found
    FlowNode* node(const Guid& id) const;
    const std::map<Guid, FlowNode*>& nodes() const;

    void add_note(GraphNote* note);
    void remove_note(GraphNote* note);

    /// Attempts to find the note with the specified id
    /// Returns nullptr if no note was found
    GraphNote* note(const Guid& id) const;
    const std::map<Guid, GraphNote*>& notes() const;

    /// Reloads all nodes in the graph
    void reload_all();
    /// Reloads all nodes of the specified class
    void reload_nodes(const char* node_class);
    
    FlowGraph(const FlowGraph& other);

private:
    std::map<Guid, FlowNode*> _nodes;
    std::map<Guid, GraphNote*> _notes;
};

class JsonObject;

namespace flow_graph
{
    FlowGraph* load(const JsonObject& root);
    void save(FlowGraph* graph, JsonObject& root);

    /// Creates a new node from a template and copies any attributes from an old node.
    ///     This requires both template and old node to have same type and node class
    /// Note: This function does not copy all links from the old node
    FlowNode* reload_node(FlowNode* tpl, FlowNode* old);
}


#endif // __FLOW_GRAPH_H__
