#include <Core/Common.h>

#include "FlowPin.h"

OBJECT_INIT_TYPE_FN(FlowPin)
{
    OBJECT_PYTHON_ADD_CLASS_ATTR("In", (int)FlowPin::In);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Out", (int)FlowPin::Out);
}

IMPLEMENT_OBJECT(FlowPin, "FlowPin", FLOW_API);

FlowPin::FlowPin()
: _pin_type(Unknown),
  _owner(nullptr),
  _id(-1)
{
}
FlowPin::FlowPin(const std::string& name,
                 Type pin_type, 
                 FlowNode* owner, 
                 int id)
: _pin_type(pin_type),
  _name(name),
  _owner(owner),
  _id(id)
{
}
FlowPin::~FlowPin()
{
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
int FlowPin::pin_id() const
{
    return _id;
}


