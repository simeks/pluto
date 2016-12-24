#include <Core/Common.h>
#include <Core/Json/JsonObject.h>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"

OBJECT_INIT_TYPE_FN(FlowGraph)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(FlowGraph, "FlowGraph", FLOW_API);

FlowGraph::FlowGraph()
{
}
FlowGraph::~FlowGraph()
{
}

void FlowGraph::add_node(FlowNode* node)
{
    assert(node);
    assert(_nodes.find(node->node_id()) == _nodes.end()); // Collision detected: Node ID already exists in graph

    if (!node->node_id().is_valid())
    {
        // Node have no ID, assign a new one
        node->set_node_id(guid::create_guid());
    }
    _nodes[node->node_id()] = node;

    node->addref();
    node->set_flow_graph(this);
}
void FlowGraph::remove_node(FlowNode* node)
{
    assert(node);
    for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        if (it->second == node)
        {
            remove_links(node);
            node->set_flow_graph(nullptr);
            node->release();
            _nodes.erase(it);
            break;
        }
    }
}
void FlowGraph::clear()
{
    _nodes.clear();
}
bool FlowGraph::try_add_link(FlowPin* a, FlowPin* b)
{
    if (!a || !b)
        return false;

    // Check pin type
    if (a->pin_type() != FlowPin::Out || b->pin_type() != FlowPin::In)
        return false;

    std::vector<Link> b_links;
    find_links(b, b_links);

    // Only one connection per input pin
    if (!b_links.empty())
        return false;

    _links.push_back(Link(a, b));
    return true;
}
void FlowGraph::remove_link(FlowPin* a, FlowPin* b)
{
    auto it = std::find(_links.begin(), _links.end(), std::pair<FlowPin*, FlowPin*>(a, b));
    if (it != _links.end())
        _links.erase(it);
}
void FlowGraph::remove_links(FlowPin* pin)
{
    auto it = _links.begin();
    while (it != _links.end())
    {
        if (it->first == pin || it->second == pin)
            it = _links.erase(it);
        else
            ++it;
    }
}
void FlowGraph::remove_links(FlowNode* node)
{
    for (auto p : node->pins())
    {
        remove_links(p);
    }
}
FlowNode* FlowGraph::node(const Guid& id) const
{
    assert(id.is_valid());
    auto it = _nodes.find(id);
    assert(it != _nodes.end());
    return it->second;
}
const std::map<Guid, FlowNode*>& FlowGraph::nodes() const
{
    return _nodes;
}
const std::vector<FlowGraph::Link>& FlowGraph::links() const
{
    return _links;
}

void FlowGraph::find_links(FlowPin* pin, std::vector<Link>& links) const
{
    for (auto l : _links)
    {
        if (l.first == pin || l.second == pin)
            links.push_back(l);
    }
}

FlowGraph* flow_graph::load(const JsonObject& root)
{
    // TODO: Validation and error handling

    FlowGraph* out_graph = object_new<FlowGraph>();

    const JsonObject& nodes = root["nodes"];
    if (!nodes.is_array())
        return nullptr;

    for (int i = 0; i < nodes.size(); ++i)
    {
        const JsonObject& n = nodes[i];
        if (!n.is_object())
            return nullptr;

        FlowNode* out_node = object_new<FlowNode>();
        std::string node_class = n["node_class"].as_string();

        out_node->set_node_id(guid::from_string(n["id"].as_string()));
        out_node->set_ui_pos(Vec2i(n["ui_pos"][0].as_int(), n["ui_pos"][1].as_int()));
            
        const JsonObject& properties = n["properties"];
        if (properties.is_array())
        {
            for (int j = 0; j < properties.size(); ++j)
            {
                const JsonObject& p = properties[j];
                if (p.is_array() && p.size() == 2)
                {
                    //out_node->set_property(p[0].as_string(), p[1].as_string());
                }
            }
        }
        out_graph->add_node(out_node);
        out_node->release();
    }

    const JsonObject& links = root["links"];
    if (links.is_array())
    {
        for (int i = 0; i < links.size(); ++i)
        {
            Guid in_node_id = guid::from_string(links[i]["in_node"].as_string());
            Guid out_node_id = guid::from_string(links[i]["out_node"].as_string());

            int in_pin_id = links["in_pin"].as_int();
            int out_pin_id = links["out_pin"].as_int();

            FlowNode* in_node = out_graph->node(in_node_id);
            FlowNode* out_node = out_graph->node(out_node_id);

            FlowPin* in_pin = in_node->pin(in_pin_id);
            FlowPin* out_pin = out_node->pin(out_pin_id);

            out_graph->try_add_link(out_pin, in_pin);
        }
    }

    return out_graph;
}
void flow_graph::save(FlowGraph* graph, JsonObject& root)
{
    graph; root;
}
