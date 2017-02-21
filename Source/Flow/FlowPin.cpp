#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"

#include <regex>

OBJECT_INIT_TYPE_FN(FlowPin)
{
    OBJECT_PYTHON_ADD_CLASS_ATTR("In", (int)FlowPin::In);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Out", (int)FlowPin::Out);
}

IMPLEMENT_OBJECT(FlowPin, "FlowPin", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowPin, Object);

namespace flow_pin
{
    std::regex pin_name_pattern("\\w+");
}

FlowPin::FlowPin(const FlowPin& other) : Object(other)
{
    _name = other._name;
    _pin_type = other._pin_type;
    _owner = other._owner;
}
FlowPin::~FlowPin()
{
}
void FlowPin::object_init()
{
    _pin_type = Unknown;
    _owner = nullptr;
}
void FlowPin::object_init(const std::string& name,
    Type pin_type,
    FlowNode* owner)
{
    if (!std::regex_match(name, flow_pin::pin_name_pattern))
        PYTHON_ERROR(ValueError, "Invalid pin name: '%s'", name.c_str());

    _name = name;
    _pin_type = pin_type;
    _owner = owner;
}
void FlowPin::object_python_init(const Tuple& args, const Dict& )
{
    if (args.size() < 2)
        PYTHON_ERROR(ValueError, "FlowPin expected at least 2 arguments");

    std::string name = python::from_python<std::string>(args.get(0));
    if (!std::regex_match(name, flow_pin::pin_name_pattern))
    {
        PYTHON_ERROR(ValueError, "Invalid pin name: '%s'", name.c_str());
    }
    _name = name;
    _pin_type = (FlowPin::Type)python::from_python<int>(args.get(1));
}
FlowPin::Type FlowPin::pin_type() const
{
    return _pin_type;
}
const char* FlowPin::name() const
{
    return _name.c_str();
}
void FlowPin::set_name(const char* name)
{
    _name = name;
}
FlowNode* FlowPin::owner() const
{
    return _owner;
}
void FlowPin::set_owner(FlowNode* node)
{
    _owner = node;
}
void FlowPin::link_to(FlowPin* other)
{
    assert(other != this);
    assert(other->_pin_type != _pin_type);
    if (other == this || other->_pin_type == _pin_type)
        return;

    if (!is_linked_to(other) && !other->is_linked_to(this))
    {
        _links.push_back(other);
        other->_links.push_back(this);
    }
}
void FlowPin::break_link(FlowPin* other)
{
    auto it = std::find(_links.begin(), _links.end(), other);
    if (it != _links.end())
    {
        _links.erase(it);
        it = std::find(other->_links.begin(), other->_links.end(), this);
        if (it != other->_links.end())
            other->_links.erase(it);
    }
}
void FlowPin::break_all_links()
{
    for (auto l : _links)
    {
        auto it = std::find(l->_links.begin(), l->_links.end(), this);
        if (it != l->_links.end())
        {
            l->_links.erase(it);
        }
    }
    _links.clear();
}
const std::vector<FlowPin*>& FlowPin::links() const
{
    return _links;
}
bool FlowPin::is_linked_to(FlowPin* other) const
{
    return std::find(_links.begin(), _links.end(), other) != _links.end();
}

OBJECT_INIT_TYPE_FN(ArrayFlowPin)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(ArrayFlowPin, "ArrayFlowPin", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(ArrayFlowPin, FlowPin);

ArrayFlowPin::ArrayFlowPin(const ArrayFlowPin& other) : FlowPin(other)
{
    _base_name = other._base_name;
    _index = other._index;
    _prev = nullptr;
    _next = nullptr;
}
ArrayFlowPin::~ArrayFlowPin()
{
}
void ArrayFlowPin::object_init()
{
    _index = -1;
    _prev = nullptr;
    _next = nullptr;
}
void ArrayFlowPin::object_init(const std::string& base_name,
    Type pin_type,
    FlowNode* owner,
    int index)
{
    if (pin_type != FlowPin::In)
        PYTHON_ERROR(ValueError, "ArrayFlowPin can only be created as an In-pin.");


    FlowPin::object_init(base_name, pin_type, owner);

    _base_name = base_name;
    _index = index;

    std::stringstream ss;
    ss << _base_name << "[" << _index << "]";
    _name = ss.str();

    _prev = nullptr;
    _next = nullptr;
}
void ArrayFlowPin::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowPin::object_python_init(args, kw);

    if (_pin_type != FlowPin::In)
        PYTHON_ERROR(ValueError, "ArrayFlowPin can only be created as an In-pin.");

    _base_name = _name;
    _index = 0; // Assume this is the first pin

    std::stringstream ss;
    ss << _base_name << "[" << _index << "]";
    _name = ss.str();

    _prev = nullptr;
    _next = nullptr;
}
const char* ArrayFlowPin::base_name() const
{
    return _base_name.c_str();
}
int ArrayFlowPin::index() const
{
    return _index;
}
void ArrayFlowPin::set_index(int index)
{
    _index = index;

    std::stringstream ss;
    ss << _base_name << "[" << _index << "]";
    _name = ss.str();
}
ArrayFlowPin* ArrayFlowPin::previous() const
{
    return _prev;
}
void ArrayFlowPin::set_previous(ArrayFlowPin* prev)
{
    _prev = prev;
}
ArrayFlowPin* ArrayFlowPin::next() const
{
    return _next;
}
void ArrayFlowPin::set_next(ArrayFlowPin* next)
{
    _next = next;
}
