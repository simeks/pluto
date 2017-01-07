#include <Core/Common.h>
#include <Core/Json/JsonObject.h>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowModule.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "RunGraphNode.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(FlowGraph, reload);

OBJECT_INIT_TYPE_FN(FlowGraph)
{
    OBJECT_PYTHON_ADD_METHOD(FlowGraph, reload, "");
}

IMPLEMENT_OBJECT(FlowGraph, "FlowGraph", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowGraph, Object);

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
    node->set_graph(this);
}
void FlowGraph::remove_node(FlowNode* node)
{
    assert(node);
    for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        if (it->second == node)
        {
            remove_links(node);
            node->set_graph(nullptr);
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

    if (a->is_linked_to(b))
        return false;

    // Only one connection per input pin
    if (!b->links().empty())
        return false;

    a->link_to(b);

    return true;
}
void FlowGraph::remove_link(FlowPin* a, FlowPin* b)
{
    a->break_link(b);
}
void FlowGraph::remove_links(FlowPin* pin)
{
    pin->break_all_links();
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
    if(it != _nodes.end())
        return it->second;
    return nullptr;
}
const std::map<Guid, FlowNode*>& FlowGraph::nodes() const
{
    return _nodes;
}
void FlowGraph::reload()
{
    for (auto& it : _nodes)
    {
        FlowNode* tpl = FlowModule::instance().node_template(it.second->node_class());
        FlowNode* old = it.second;
        it.second = flow_graph::reload_node(tpl, old);
        old->release();
    }
}
void FlowGraph::reload(const char* node_class)
{
    FlowNode* tpl = FlowModule::instance().node_template(node_class);

    std::vector<FlowPin*> links;
    for (auto& it : _nodes)
    {
        if (strcmp(it.second->node_class(), node_class) == 0)
        {
            FlowNode* old = it.second;
            FlowNode* new_node = flow_graph::reload_node(tpl, old);
            if (!new_node)
            {
                // TODO:
                std::cout << "Reloading node " << guid::to_string(old->node_id()) 
                    << " failed (node_class: " << node_class << ")" << std::endl;
                continue;
            }
            it.second = new_node;
            
            for (auto& old_pin : old->pins())
            {
                FlowPin* new_pin = new_node->pin(old_pin->name());

                links = old_pin->links();
                old_pin->break_all_links();

                if (new_pin)
                {
                    for (FlowPin* l : links)
                    {
                        new_pin->link_to(l);
                    }
                }
            }
            old->release();
        }

        // Special case: Update any embedded graphs as well
        if (it.second->is_a(RunGraphNode::static_class()))
        {
            RunGraphNode* n = object_cast<RunGraphNode>(it.second);
            if (n->graph() != this)
                n->graph()->reload(node_class);
        }
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
        if (properties.is_object())
        {
            for (auto& p : properties)
            {
                if (p.second.is_string())
                    out_node->set_property(p.first.c_str(), p.second.as_string().c_str());
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

    /// Links pointing from first pin to second pin
    typedef std::pair<FlowPin*, FlowPin*> Link;
    std::vector<Link> node_links;

    JsonObject& nodes = root["nodes"];
    nodes.set_empty_array();
    for (auto& n : graph->nodes())
    {
        JsonObject& node = nodes.append();
        node.set_empty_object();

        node["id"].set_string(guid::to_string(n.second->node_id()));
        node["node_class"].set_string(n.second->node_class());
        node["ui_pos"].set_empty_array();

        Vec2i pos = n.second->ui_pos();
        node["ui_pos"].append().set_int(pos.x);
        node["ui_pos"].append().set_int(pos.y);

        JsonObject& properties = node["properties"];
        properties.set_empty_object();
        for (auto p : n.second->properties())
        {
            properties[p->name()].set_string(n.second->property(p->name()));
        }

        for (auto outpin : n.second->pins())
        {
            if (outpin->pin_type() == FlowPin::Out)
            {
                for (auto inpin : outpin->links())
                {
                    node_links.push_back(Link(outpin, inpin));
                }
            }
        }
    }

    JsonObject& links = root["links"];
    links.set_empty_array();
    for (Link& l : node_links)
    {
        JsonObject& link = links.append();
        link.set_empty_object();

        link["begin_node"].set_string(guid::to_string(l.first->owner()->node_id()));
        link["end_node"].set_string(guid::to_string(l.second->owner()->node_id()));

        link["begin_pin"].set_int(l.first->pin_id());
        link["end_pin"].set_int(l.second->pin_id());
    }
}
FlowNode* flow_graph::reload_node(FlowNode* tpl, FlowNode* old)
{
    if (strcmp(tpl->node_class(), old->node_class()) != 0)
        return nullptr;

    FlowNode* n = object_clone(tpl);
    n->set_node_id(old->node_id());
    n->set_graph(old->graph());
    n->set_ui_pos(old->ui_pos());
    for (auto p : old->properties())
    {
        n->set_attribute(p->name(), old->attribute(p->name()));
    }

    return n;
}

