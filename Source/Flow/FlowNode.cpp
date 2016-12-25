#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowNode, run, FlowContext*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowNode, add_pin, std::string, int);

OBJECT_INIT_TYPE_FN(FlowNode)
{
    OBJECT_PYTHON_ADD_METHOD(FlowNode, run, "");
    OBJECT_PYTHON_ADD_METHOD(FlowNode, add_pin, "");
}

IMPLEMENT_OBJECT(FlowNode, "FlowNode", FLOW_API);

FlowNode::FlowNode() :
    _owner_graph(nullptr), 
    _function(nullptr)
{
}
FlowNode::FlowNode(const FlowNodeDef& def) :
    _owner_graph(nullptr)
{
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
FlowNode::~FlowNode()
{
    _pins.clear();
}
void FlowNode::run(FlowContext* ctx)
{
    PyObject* method = PyObject_GetAttrString(_py_object, "run");
    if (!PyCFunction_Check(method))
    {
        // To avoid recursion we only call "run" if it is actually overriden by Python.
        PyObject* ret = PyObject_Call(method, python_helpers::build_args(ctx), nullptr);
        if (!ret)
            PyErr_Print();
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
FlowPin* FlowNode::pin(const std::string& name) const
{
    for (auto& p : _pins)
    {
        if (p->name() == name)
        {
            return p;
        }
    }
    return nullptr;
}

const Guid& FlowNode::node_id() const
{
    return _node_id;
}
void FlowNode::set_node_id(const Guid& id)
{
    _node_id = id;
}
void FlowNode::set_flow_graph(FlowGraph* graph)
{
    _owner_graph = graph;
}

void FlowNode::add_pin(const std::string& name, int pin_type)
{
    int id = (int)_pins.size();
    _pins.push_back(object_new<FlowPin>(name, (FlowPin::Type)pin_type, this, id));
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

FlowNode::FlowNode(const FlowNode& other)
{
    _py_object = other._py_object;
    Py_XINCREF(_py_object);

    for (auto& pin : other._pins)
    {
        add_pin(pin->name(), pin->pin_type());
    }
    _owner_graph = other._owner_graph;
    _node_id = other._node_id;
    _function = other._function;
}
FlowNode& FlowNode::operator=(const FlowNode& other)
{
    _py_object = other._py_object;
    Py_XINCREF(_py_object);

    _pins.clear();
    for (auto& pin : other._pins)
    {
        add_pin(pin->name(), pin->pin_type());
    }
    _owner_graph = other._owner_graph;
    _node_id = other._node_id;
    _function = other._function;

    return *this;
}