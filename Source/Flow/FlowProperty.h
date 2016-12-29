#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowProperty : public Object
{
    DECLARE_OBJECT(FlowProperty, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(FlowProperty);
    ~FlowProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);

    const char* name() const;
    const char* default_value() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

private:
    std::string _name;
    std::string _default_value;

    FlowNode* _owner;
};

#endif // __FLOW_PROPERTY_H__
