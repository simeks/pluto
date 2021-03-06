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
    FlowPin(const FlowPin& other);
    virtual ~FlowPin();
    
    void object_init();

    /// @param name : Can only contain a-z, A-Z, 0-9, '_'. Remember, pin names are case insensitive
    void object_init(const std::string& name,
                     Type pin_type,
                     FlowNode* owner);
    void object_python_init(const Tuple& args, const Dict&);

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
    DECLARE_OBJECT(ArrayFlowPin, FlowPin);

public:
    DECLARE_OBJECT_CONSTRUCTOR(ArrayFlowPin);
    ArrayFlowPin(const ArrayFlowPin& other);
    virtual ~ArrayFlowPin();

    void object_init();

    /// @param base_name : Can only contain a-z, A-Z, 0-9, '_'. Remember, pin names are case insensitive
    void object_init(const std::string& base_name,
                     Type pin_type,
                     FlowNode* owner,
                     int index = 0);
    void object_python_init(const Tuple& args, const Dict&);

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
