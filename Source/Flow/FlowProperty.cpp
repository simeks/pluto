#include <Core/Common.h>

#include "FlowProperty.h"

OBJECT_INIT_TYPE_FN(FlowProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(FlowProperty, "FlowProperty", FLOW_API);

FlowProperty::FlowProperty() : 
    _name(""),
    _default_value(""),
    _owner(nullptr)
{
}
FlowProperty::~FlowProperty()
{
}
FlowNode* FlowProperty::owner() const
{
    return _owner;
}
void FlowProperty::set_owner(FlowNode* node)
{
    _owner = node;
}
int FlowProperty::object_init(const Tuple& args, const Dict&)
{
    if (args.size() == 1)
        _name = python_convert::from_python<std::string>(args.get(0));
    if (args.size() > 1)
        _default_value = python_convert::from_python<std::string>(args.get(1));
    return 0;
}

