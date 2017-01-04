#include <Core/Common.h>

#include "FlowProperty.h"

OBJECT_INIT_TYPE_FN(FlowProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(FlowProperty, "FlowProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowProperty, Object);

FlowProperty::~FlowProperty()
{
}
void FlowProperty::object_init()
{
    _owner = nullptr;
}
void FlowProperty::object_init(const char* name, const char* default_value)
{
    _owner = nullptr;
    _name = name;
    _default_value = default_value;
}
void FlowProperty::object_python_init(const Tuple& args, const Dict&)
{
    _owner = nullptr;
    if (args.size() > 0)
        _name = python_convert::from_python<std::string>(args.get(0));
    if (args.size() > 1)
        _default_value = python_convert::from_python<std::string>(args.get(1));
}
const char* FlowProperty::name() const
{
    return _name.c_str();
}
const char* FlowProperty::default_value() const
{
    return _default_value.c_str();
}
FlowNode* FlowProperty::owner() const
{
    return _owner;
}
void FlowProperty::set_owner(FlowNode* node)
{
    _owner = node;
}