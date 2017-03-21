#include <Core/Common.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Python/Sequence.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "FlowProperty.h"

PYTHON_OBJECT_IMPL_DOC(FlowNode, "Node", 
    "FlowNode\n"
    "Base class for nodes\n"
    "\n"
    "Attributes:\n"
    "   node_class (str): Unique class name for this type of node.\n"
    "   title (str): Title visible in the UI\n"
    "   category (str): Category of the form 'Category/Subcategory'\n")
{
    cls.def_init<FlowNode>();
    cls.def("run", &FlowNode::run, 
        "run(ctx)\n"
        "--\n"
        "\n"
        "Args:\n"
        "   ctx : Context\n");
    // TODO: Template args specifies which add_pin to use, ugly so probably needs rethinking
    cls.def<FlowNode, void, const char*, int>("add_pin", &FlowNode::add_pin, "");
    cls.def("node_id", &FlowNode::node_id, "");
    cls.def("is_pin_linked", &FlowNode::is_pin_linked, "");
}

FlowNode::FlowNode() :
    _owner_graph(nullptr),
    _function(nullptr)
{
}
FlowNode::FlowNode(const FlowNodeDef& def) :
    _owner_graph(nullptr),
    _function(def.fn)
{
    if (def.pins)
    {
        FlowPinDef* pin = def.pins;
        while (pin->name)
        {
            // TODO: Pin doc
            add_pin(pin->name, pin->type);
            ++pin;
        }
    }
}
FlowNode::~FlowNode()
{
    for (auto p : _pins)
    {
        p->release();
    }
    _pins.clear();
    for (auto p : _properties)
    {
        p->release();
    }
    _properties.clear();
}
void FlowNode::run(FlowContext* ctx)
{
    python::Object method = attribute("run");
    if (is_overridden(method))
    {
        // To avoid recursion we only call "run" if it is actually overriden by Python.
        method(python::make_tuple(ctx));
    }
    else if (_function)
    {
        _function(ctx);
    }
}
const std::vector<FlowPin*>& FlowNode::pins() const
{
    return _pins;
}
FlowPin* FlowNode::pin(const char* name) const
{
    for (auto& p : _pins)
    {
        // Case-insensitive comparison
        if (_stricmp(p->name(), name) == 0)
        {
            return p;
        }
    }
    return nullptr;
}
ArrayFlowPin* FlowNode::pin(const char* name, int index)
{
    std::vector<ArrayFlowPin*> pins = pin_array(name);
    for (auto& p : pins)
    {
        if (p->index() == index)
            return p;
    }

    if (pins.empty())
        return nullptr;

    ArrayFlowPin* last = pins.back();
    while (last->index() < index)
    {
        ArrayFlowPin* pin = python::clone_object(last);
        pin->set_index(last->index() + 1);
        pin->set_previous(last);
        last->set_next(pin);

        pin->set_owner(this);
        _pins.insert(std::find(_pins.begin(), _pins.end(), last) + 1, pin);

        last = pin;
    }

    return last;
}
std::vector<ArrayFlowPin*> FlowNode::pin_array(const char* name) const
{
    std::vector<ArrayFlowPin*> out;
    for (auto& p : _pins)
    {
        if (p->is_a(ArrayFlowPin::static_class()))
        {
            ArrayFlowPin* pin = python::object_cast<ArrayFlowPin>(p);

            // Case-insensitive comparison
            if (_stricmp(pin->base_name(), name) == 0)
            {
                // We can assume that all pins come in order
                out.push_back(pin);
            }
        }
    }

    return out;
}
bool FlowNode::is_pin_linked(const char* name) const
{
    FlowPin* p = pin(name);
    return p && p->links().size() != 0;
}

const Guid& FlowNode::node_id() const
{
    return _node_id;
}
void FlowNode::set_node_id(const Guid& id)
{
    _node_id = id;
}
FlowGraph* FlowNode::graph() const
{
    return _owner_graph;
}
void FlowNode::set_graph(FlowGraph* graph)
{
    _owner_graph = graph;
}

void FlowNode::add_pin(const char* name, int pin_type)
{
    _pins.push_back(python::make_object<FlowPin>(name, (FlowPin::Type)pin_type, this));
}
void FlowNode::add_pin(FlowPin* pin)
{
    pin->set_owner(this);
    _pins.push_back(pin);
}
void FlowNode::add_property(FlowProperty* prop)
{
    prop->set_owner(this);
    _properties.push_back(prop);

    set_attribute(prop->name(), prop->default_value());
}
const std::vector<FlowProperty*>& FlowNode::properties() const
{
    return _properties;
}
void FlowNode::set_property(const char* name, const char* value)
{
    set_attribute(name, value);
}
void FlowNode::set_property(const char* name, const python::Object& value)
{
    set_attribute(name, value);
}

const char* FlowNode::node_class() const
{
    if (has_attribute("node_class"))
        return attribute<const char*>("node_class");
    return nullptr;
}
const char* FlowNode::title() const
{
    if (has_attribute("title"))
        return attribute<const char*>("title");
    return nullptr;
}
const char* FlowNode::category() const
{
    if (has_attribute("category"))
        return attribute<const char*>("category");
    return "";
}
const Vec2i& FlowNode::ui_pos() const
{
    return _ui_pos;
}
void FlowNode::set_ui_pos(const Vec2i& pos)
{
    _ui_pos = pos;
}
const char* FlowNode::ui_class() const
{
    if (has_attribute("ui_class"))
        return attribute<const char*>("ui_class");
    return "";
}
FlowNode::FlowNode(const FlowNode& other) : python::BaseObject(other)
{
    for (auto& pin : other._pins)
    {
        FlowPin* p = python::clone_object(pin);
        add_pin(p);
    }
    for (auto& prop : other._properties)
    {
        FlowProperty* p = python::clone_object(prop);
        add_property(p);

        set_attribute(p->name(), other.attribute(p->name()));
    }
    _owner_graph = other._owner_graph;
    _node_id = other._node_id;
    _function = other._function;
}
void FlowNode::on_pin_linked(FlowPin* pin)
{
    if (pin->is_a(ArrayFlowPin::static_class()))
    {
        add_array_pin(python::object_cast<ArrayFlowPin>(pin));
    }
}
void FlowNode::on_pin_unlinked(FlowPin* pin)
{
    if (pin->is_a(ArrayFlowPin::static_class()))
    {
        remove_array_pin(python::object_cast<ArrayFlowPin>(pin));
    }
}
void FlowNode::add_array_pin(ArrayFlowPin* prev)
{
    ArrayFlowPin* first = prev;
    while (first->previous()) first = first->previous();

    ArrayFlowPin* last = first;

    int n = 0; // Number of free pins
    while (last->next())
    {
        if (last->links().size() == 0)
            ++n;

        last = last->next();
    }

    if (n > 0)
        return; // Don't add any new pins to array if we already have free pins
    
    ArrayFlowPin* pin = python::clone_object(last);
    pin->set_index(last->index() + 1);
    pin->set_previous(last);
    last->set_next(pin);

    pin->set_owner(this);
    _pins.insert(std::find(_pins.begin(), _pins.end(), last)+1, pin);
}
void FlowNode::remove_array_pin(ArrayFlowPin* pin)
{
    // Unlink pin from others
    ArrayFlowPin* next = pin->next();
    ArrayFlowPin* prev = pin->previous();

    if (next) next->set_previous(prev);
    if (prev) prev->set_next(next);

    // Decrement indices of pins after this one
    while (next)
    {
        next->set_index(next->index() - 1);
        next = next->next();
    }
    
    // Remove pin from node
    auto it = std::find(_pins.begin(), _pins.end(), pin);
    assert(it != _pins.end());
    _pins.erase(it);

    pin->release();
}
