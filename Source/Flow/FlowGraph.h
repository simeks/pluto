#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__

#include <Core/Python/BaseObject.h>
#include <Core/Image/Vec2.h>

#include "API.h"

class FlowGraph;
class FlowNode;
class FlowPin;

struct GraphNote
{
    GraphNote();

    FlowGraph* owner;
    Guid id;

    std::string text;
    Vec2i pos;
};

class FLOW_API FlowGraph : public python::BaseObject
{
    PYTHON_OBJECT(FlowGraph, python::BaseObject);

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

    FlowNode* node(const Guid& id) const;
    const std::map<Guid, FlowNode*>& nodes() const;

    /// Adds a new note with the given properties to the graph and returns the id to said note.
    Guid make_note(const char* text=nullptr, const Vec2i& pos = Vec2i(0, 0));

    /// Similar to make_note but for when the user already have a desired id.
    /// Fails if a note with given id already exists
    void insert_note(const Guid& id, const char* text, const Vec2i& pos);

    /// Removes the note with the given id
    /// Fails if no note with the given id exists
    void remove_note(const Guid& id);


    Vec2i note_position(const Guid& id) const;
    const char* note_text(const Guid& id) const;

    void set_note_position(const Guid& id, const Vec2i& pos);
    void set_note_text(const Guid& id, const char* text);
    
    const std::map<Guid, GraphNote>& notes() const;

    /// Reloads all nodes in the graph
    void reload_all();
    /// Reloads all nodes of the specified class
    void reload_nodes(const char* node_class);
    
    FlowGraph(const FlowGraph& other);

private:
    std::map<Guid, FlowNode*> _nodes;
    std::map<Guid, GraphNote> _notes;
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
