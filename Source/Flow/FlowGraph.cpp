#include <Core/Common.h>
#include <Core/Json/JsonObject.h>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowModule.h"
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

        std::string node_class = n["node_class"].as_string();
        FlowNode* tpl = FlowModule::instance().node_template(node_class.c_str());
        if (!tpl)
        {
            // TODO: Error handling
            continue;
        }
        FlowNode* out_node = object_clone(tpl);

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
            Guid begin_node_id = guid::from_string(links[i]["begin_node"].as_string());
            Guid end_node_id = guid::from_string(links[i]["end_node"].as_string());

            int begin_pin_id = links[i]["begin_pin"].as_int();
            int end_pin_id = links[i]["end_pin"].as_int();

            FlowNode* begin_node = out_graph->node(begin_node_id);
            FlowNode* end_node = out_graph->node(end_node_id);

            // TODO: Error handling
            // Nodes not found most likely caused by template not found while parsing nodes
            if (!begin_node || !end_node)
                continue;

            FlowPin* begin_pin = begin_node->pin(begin_pin_id);
            FlowPin* end_pin = end_node->pin(end_pin_id);

            out_graph->try_add_link(begin_pin, end_pin);
        }
    }

    return out_graph;
}
void flow_graph::save(FlowGraph* graph, JsonObject& root)
{
    root.set_empty_object();

    JsonObject& nodes = root["nodes"];
    nodes.set_empty_array();
    for (auto n : graph->nodes())
    {
        JsonObject& node = nodes.append();
        node.set_empty_object();

        node["id"].set_string(guid::to_string(n.second->node_id()));
        node["node_class"].set_string(n.second->node_class());
        node["ui_pos"].set_empty_array();

        Vec2i pos = n.second->ui_pos();
        node["ui_pos"].append().set_int(pos.x);
        node["ui_pos"].append().set_int(pos.y);

        // TODO: Properties
    }

    JsonObject& links = root["links"];
    links.set_empty_array();
    for (const FlowGraph::Link& l : graph->links())
    {
        JsonObject& link = links.append();
        link.set_empty_object();

        link["begin_node"].set_string(guid::to_string(l.first->owner()->node_id()));
        link["end_node"].set_string(guid::to_string(l.second->owner()->node_id()));

        link["begin_pin"].set_int(l.first->pin_id());
        link["end_pin"].set_int(l.second->pin_id());
    }
}
