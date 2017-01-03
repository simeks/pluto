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

    DECLARE_OBJECT_CONSTRUCTOR(FlowPin);
    virtual ~FlowPin();
    
    void object_init();
    void object_init(const std::string& name,
                     Type pin_type,
                     FlowNode* owner, 
                     int id);
    void object_python_init(const Tuple& args, const Dict&);

    Type pin_type() const;
    const std::string& name() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

    int pin_id() const;
    void set_pin_id(int id);

    void link_to(FlowPin* other);
    void break_link(FlowPin* other);
    void break_all_links();

    const std::vector<FlowPin*>& links() const;
    bool is_linked_to(FlowPin* other) const;

private:
    Type _pin_type;
    std::string _name;

    FlowNode* _owner;
    int _id;

    std::vector<FlowPin*> _links;
};

#endif // __FLOW_PIN_H__
