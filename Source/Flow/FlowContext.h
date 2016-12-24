#ifndef __FLOW_CONTEXT_H__
#define __FLOW_CONTEXT_H__

#include "API.h"
#include <Core/Object/Object.h>

class FLOW_API FlowContext : public Object
{
    DECLARE_OBJECT(FlowContext, Object);

public:
    FlowContext();
    ~FlowContext();


};

template<typename ObjectType>
ObjectType* FlowContext::allocate_object()
{
    ObjectType* obj = new ObjectType();
    objects.push_back(obj);
    return obj;
}

template<typename T>
T* FlowContext::read_pin(const std::string& pin_name)
{
    return object_cast<T>(read_pin(pin_name));
}


#endif // __FLOW_CONTEXT_H__
