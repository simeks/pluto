#ifndef __FLOW_PIN_H__
#define __FLOW_PIN_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowPin : public Object
{
    DECLARE_OBJECT(FlowPin, Object);

public:
    enum Type
    {
        In,
        Out,
        Unknown
    };

    FlowPin();
    FlowPin(const std::string& name,
            Type pin_type,
            FlowNode* owner, int id);
    virtual ~FlowPin();

    Type pin_type() const;
    const std::string& name() const;

    FlowNode* owner() const;
    
    int pin_id() const;
    void set_pin_id(int id);

    int object_init(const Tuple&, const Dict&) OVERRIDE;
private:
    Type _pin_type;
    std::string _name;

    FlowNode* _owner;
    int _id;
};

#endif // __FLOW_PIN_H__
