#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"

#include <regex>

PYTHON_OBJECT_IMPL_DOC(FlowPin, "Pin", 
    "Pin(type, name=''): Creates a Pin of specified type (Pin.In or Pin.Out)\n")
{
    cls.def_init_varargs<FlowPin>();
    cls.def("In", (int)FlowPin::In);
    cls.def("Out", (int)FlowPin::Out);
}

namespace flow_pin
{
    std::regex pin_name_pattern("\\w+");
}

FlowPin::FlowPin(
    const std::string& name,
    Type pin_type,
    FlowNode* owner) : 
    _name(name),
    _pin_type(pin_type), 
    _owner(owner)
{
    if (!std::regex_match(name, flow_pin::pin_name_pattern))
        PYTHON_ERROR(PyExc_ValueError, "Invalid pin name: '%s'", name.c_str());
}
FlowPin::FlowPin() : _pin_type(Unknown), _owner(nullptr)
{
}
FlowPin::FlowPin(const FlowPin& other) : python::BaseObject(other)
{
    _name = other._name;
    _pin_type = other._pin_type;
    _owner = other._owner;
}
FlowPin::FlowPin(const python::Tuple& args)
{
    if (args.size() < 1)
        PYTHON_ERROR(PyExc_ValueError, "Pin expected at least 1 argument");

    // FlowPin(type, name="")

    _name = "";
    _pin_type = (FlowPin::Type)args.get<int>(0);
    
    if (args.size() > 1)
    {
        std::string name = args.get<const char*>(1);
        if (!std::regex_match(name, flow_pin::pin_name_pattern))
        {
            PYTHON_ERROR(PyExc_ValueError, "Invalid pin name: '%s'", name.c_str());
        }
        _name = name;
    }
}
FlowPin::~FlowPin()
{
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

PYTHON_OBJECT_IMPL(ArrayFlowPin, "ArrayPin")
{
    cls.def_init_varargs<ArrayFlowPin>();
}

ArrayFlowPin::ArrayFlowPin(const std::string& base_name,
                           Type pin_type,
                           FlowNode* owner,
                           int index) :
    FlowPin(base_name, pin_type, owner),
    _prev(nullptr),
    _next(nullptr)
{
    if (pin_type != FlowPin::In)
        PYTHON_ERROR(PyExc_ValueError, "ArrayFlowPin can only be created as an In-pin.");

    _base_name = base_name;
    _index = index;

    std::stringstream ss;
    ss << _base_name << "[" << _index << "]";
    _name = ss.str();

    _prev = nullptr;
    _next = nullptr;
}
ArrayFlowPin::ArrayFlowPin(const python::Tuple& args) : FlowPin(args)
{
    if (_pin_type != FlowPin::In)
        PYTHON_ERROR(PyExc_ValueError, "ArrayFlowPin can only be created as an In-pin.");

    _base_name = _name;
    _index = 0; // Assume this is the first pin

    std::stringstream ss;
    ss << _base_name << "[" << _index << "]";
    _name = ss.str();

    _prev = nullptr;
    _next = nullptr;
}
ArrayFlowPin::ArrayFlowPin() : 
    _index(-1),
    _prev(nullptr),
    _next(nullptr)
{
}
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
