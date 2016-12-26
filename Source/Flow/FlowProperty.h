#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowProperty : public Object
{
    DECLARE_OBJECT(FlowProperty, Object);

public:
    FlowProperty();
    virtual ~FlowProperty();

    const char* name() const;
    const char* default_value() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

    int object_init(const Tuple&, const Dict&) OVERRIDE;

private:
    std::string _name;
    std::string _default_value;

    FlowNode* _owner;
};

#endif // __FLOW_PROPERTY_H__
