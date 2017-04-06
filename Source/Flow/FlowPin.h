#ifndef __FLOW_PIN_H__
#define __FLOW_PIN_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowPin : public Object
{
    PLUTO_OBJECT(FlowPin, Object);

public:
    enum Type
    {
        In,
        Out,
        Unknown
    };
    
    /// @param name : Can only contain a-z, A-Z, 0-9, '_'. Remember, pin names are case insensitive
    FlowPin(const std::string& name,
            Type pin_type,
            FlowNode* owner); 

    /// Pin(type, name="")
    FlowPin(const python::Tuple& args);
    
    FlowPin();
    FlowPin(const FlowPin& other);
    virtual ~FlowPin();
    
    Type pin_type() const;

    const char* name() const;
    void set_name(const char* name);

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

    void link_to(FlowPin* other);
    void break_link(FlowPin* other);
    void break_all_links();

    const std::vector<FlowPin*>& links() const;
    bool is_linked_to(FlowPin* other) const;

protected:
    Type _pin_type;
    std::string _name;

    FlowNode* _owner;
    
    std::vector<FlowPin*> _links;
};


class FLOW_API ArrayFlowPin : public FlowPin
{
    PLUTO_OBJECT(ArrayFlowPin, FlowPin);

public:
    /// @param base_name : Can only contain a-z, A-Z, 0-9, '_'. Remember, pin names are case insensitive
    ArrayFlowPin(const std::string& base_name,
        Type pin_type,
        FlowNode* owner,
        int index = 0);

    /// ArrayPin(type, name="")
    ArrayFlowPin(const python::Tuple& args);

    ArrayFlowPin();
    ArrayFlowPin(const ArrayFlowPin& other);
    virtual ~ArrayFlowPin();


    const char* base_name() const;

    int index() const;
    void set_index(int index);

    ArrayFlowPin* previous() const;
    void set_previous(ArrayFlowPin* prev);

    ArrayFlowPin* next() const;
    void set_next(ArrayFlowPin* next);

protected:
    std::string _base_name; // Name before []
    int _index; // Index in array

    ArrayFlowPin* _prev;
    ArrayFlowPin* _next;
};

#endif // __FLOW_PIN_H__
