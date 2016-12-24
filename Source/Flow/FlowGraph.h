#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FlowPin;
class FLOW_API FlowGraph : public Object
{
    DECLARE_OBJECT(FlowGraph, Object);

public:
    /// Links pointing from first pin to second pin
    typedef std::pair<FlowPin*, FlowPin*> Link;

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

    FlowNode* node(const Guid& id) const;
    const std::map<Guid, FlowNode*>& nodes() const;
    const std::vector<Link>& links() const;

private:

    /// Find links associated with the specified pin
    void find_links(FlowPin* pin, std::vector<Link>& links) const;

    std::map<Guid, FlowNode*> _nodes;

    std::vector<Link> _links;
};

class JsonObject;
namespace flow_graph
{
    FlowGraph* load(const JsonObject& root);
    void save(FlowGraph* graph, JsonObject& root);
}


#endif // __FLOW_GRAPH_H__
