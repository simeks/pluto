#include <Core/Common.h>

#include "FlowPin.h"

OBJECT_INIT_TYPE_FN(FlowPin)
{
    OBJECT_PYTHON_ADD_CLASS_ATTR("In", (int)FlowPin::In);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Out", (int)FlowPin::Out);
}

IMPLEMENT_OBJECT(FlowPin, "FlowPin", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowPin, Object);

FlowPin::~FlowPin()
{
}
void FlowPin::object_init()
{
    _pin_type = Unknown;
    _owner = nullptr;
    _id = -1;
}
void FlowPin::object_init(const std::string& name,
    Type pin_type,
    FlowNode* owner,
    int id)
{
    _name = name;
    _pin_type = pin_type;
    _owner = owner;
    _id = id;
}
void FlowPin::object_python_init(const Tuple& args, const Dict&)
{
    if (args.size() == 2)
    {
        _name = python_convert::from_python<std::string>(args.get(0));
        _pin_type = (FlowPin::Type)python_convert::from_python<int>(args.get(1));
    }
}
FlowPin::Type FlowPin::pin_type() const
{
    return _pin_type;
}
const std::string& FlowPin::name() const
{
    return _name;
}
FlowNode* FlowPin::owner() const
{
    return _owner;
}
void FlowPin::set_owner(FlowNode* node)
{
    _owner = node;
}
int FlowPin::pin_id() const
{
    return _id;
}
void FlowPin::set_pin_id(int id)
{
    _id = id;
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
            l->_links.erase(it);
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


