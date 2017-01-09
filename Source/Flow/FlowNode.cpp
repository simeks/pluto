#include <Core/Common.h>
#include <Core/Python/Sequence.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "FlowProperty.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowNode, run, FlowContext*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowNode, add_pin, const char*, int);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowNode, node_id);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowNode, is_pin_linked, const char*);

OBJECT_INIT_TYPE_FN(FlowNode)
{
    OBJECT_PYTHON_ADD_METHOD(FlowNode, run, "");
    OBJECT_PYTHON_ADD_METHOD(FlowNode, add_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowNode, node_id, "");
    OBJECT_PYTHON_ADD_METHOD(FlowNode, is_pin_linked, "");
}

IMPLEMENT_OBJECT(FlowNode, "FlowNode", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowNode, Object);

void FlowNode::object_init()
{
    _owner_graph = nullptr;
    _function = nullptr;
}
void FlowNode::object_init(const FlowNodeDef& def)
{
    _owner_graph = nullptr;
    _function = def.fn;

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
void FlowNode::object_python_init(const Tuple&, const Dict&)
{
    _owner_graph = nullptr;
    _function = nullptr;

    Dict d = get_class()->dict();
    if (d.has_key("pins"))
    {
        Sequence pins = Sequence(d.get("pins"));
        for (size_t i = 0; i < pins.size(); ++i)
        {
            FlowPin* pin = python_convert::from_python<FlowPin*>(pins.get(i));
            add_pin(object_clone(pin));
        }
    }
    if (d.has_key("properties"))
    {
        Sequence props = Sequence(d.get("properties"));
        for (size_t i = 0; i < props.size(); ++i)
        {
            FlowProperty* prop = python_convert::from_python<FlowProperty*>(props.get(i));
            add_property(object_clone(prop));
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
    PyObject* method = PyObject_GetAttrString(_py_object, "run");
    if (!PyCFunction_Check(method))
    {
        // To avoid recursion we only call "run" if it is actually overriden by Python.
        PyObject_Call(method, python_helpers::build_args(ctx), nullptr);
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
FlowPin* FlowNode::pin(int id) const
{
    return _pins[id];
}
FlowPin* FlowNode::pin(const char* name) const
{
    for (auto& p : _pins)
    {
        if (strcmp(p->name(), name) == 0)
        {
            return p;
        }
    }
    return nullptr;
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
    int id = (int)_pins.size();
    _pins.push_back(object_new<FlowPin>(name, (FlowPin::Type)pin_type, this, id));
}
void FlowNode::add_pin(FlowPin* pin)
{
    int id = (int)_pins.size();
    pin->set_pin_id(id);
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
const char* FlowNode::property(const char* name) const
{
    return attribute<const char*>(name);
}
void FlowNode::set_property(const char* name, const char* value)
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
FlowNode::FlowNode(const FlowNode& other) : Object(other)
{
    for (auto& pin : other._pins)
    {
        add_pin(pin->name(), pin->pin_type());
    }
    for (auto& prop : other._properties)
    {
        FlowProperty* p = object_clone(prop);
        p->set_owner(this);
        _properties.push_back(p);
    }
    _owner_graph = other._owner_graph;
    _node_id = other._node_id;
    _function = other._function;
}
