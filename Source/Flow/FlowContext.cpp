#include <Core/Common.h>

#include "FlowContext.h"

OBJECT_INIT_TYPE_FN(FlowContext)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(FlowContext, "FlowContext", FLOW_API);

FlowContext::FlowContext()
{
}
FlowContext::~FlowContext()
{
}
