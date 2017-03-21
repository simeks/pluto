#ifndef __FLOW_NODE_H__
#define __FLOW_NODE_H__

#include <Core/Image/Vec2.h>
#include <Core/Python/BaseObject.h>

#include "API.h"

class QtFlowNode;

class ArrayFlowPin;
class FlowContext;
class FlowGraph;
class FlowPin;
class FlowProperty;

typedef void(*FlowNodeFunction)(FlowContext*);

struct FlowPinDef
{
    const char* name;
    int type;
    const char* doc;
};

struct FlowNodeDef
{
    const char* class_name;
    const char* title;
    const char* category;
    FlowPinDef* pins;
    FlowNodeFunction fn;
    const char* doc;
};


class FLOW_API FlowNode : public python::BaseObject
{
    PYTHON_OBJECT(FlowNode, python::BaseObject);

public:
    FlowNode();
    FlowNode(const FlowNodeDef& def);
    ~FlowNode();

    virtual void run(FlowContext* ctx);

    const std::vector<FlowPin*>& pins() const;

    /// Note: Pin names are case-insensitive
    FlowPin* pin(const char* name) const;

    /// Returns the array pin with the given base name and array index
    /// Returns a new pin if the array exists but does not cover the specified index.
    /// Returns null if no array pin with the given name was found.
    ArrayFlowPin* pin(const char* name, int index);

    /// Returns all pins of the array with the given name
    std::vector<ArrayFlowPin*> pin_array(const char* name) const;

    /// Note: Pin names are case-insensitive
    bool is_pin_linked(const char* name) const;

    const Guid& node_id() const;
    void set_node_id(const Guid& id);

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);

    const char* node_class() const;
    const char* title() const;
    const char* category() const;

    void add_pin(const char* name, int pin_type);
    void add_pin(FlowPin* pin);

    const std::vector<FlowProperty*>& properties() const;

    void set_property(const char* name, const char* value);
    void set_property(const char* name, const python::Object& value);

    const Vec2i& ui_pos() const;
    void set_ui_pos(const Vec2i& pos);

    const char* ui_class() const;

    FlowNode(const FlowNode&);

    /// Called whenever the given pin receives a new link
    void on_pin_linked(FlowPin* pin);
    /// Called whenever the given pin is unlinked from another pin
    void on_pin_unlinked(FlowPin* pin);

protected:
    /// Steals the reference to prop
    void add_property(FlowProperty* prop);

    /// Adds a new array pin after the specified pin
    void add_array_pin(ArrayFlowPin* prev);

    /// Removes an array pin updates all other pins accordingly
    void remove_array_pin(ArrayFlowPin* pin);

    std::vector<FlowPin*> _pins;
    std::vector<FlowProperty*> _properties;

    FlowGraph* _owner_graph;
    Guid _node_id;
    Vec2i _ui_pos;

    FlowNodeFunction _function;
};




#endif // __FLOW_NODE_H__
