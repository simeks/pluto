#ifndef __FLOW_NODE_H__
#define __FLOW_NODE_H__

#include <Core/Image/Vec2.h>
#include <Core/Object/Object.h>

#include "API.h"

class QtFlowNode;

class ArrayFlowPin;
class FlowContext;
class FlowGraph;
class FlowPin;
class FlowProperty;

typedef void(*FlowNodeFunction)(FlowContext*);

class FLOW_API FlowNode : public Object
{
    PLUTO_OBJECT(FlowNode, Object);

public:
    FlowNode();
    FlowNode(
        const char* node_class,
        const char* title,
        const char* category);
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

    /// Ïncreases the refcount of prop
    void add_property(FlowProperty* prop);

    /// Returns the property with the given name, returns null if not found
	/// @remark case-insensitive
    FlowProperty* property(const char* name) const;

    /// Returns the value for the specified property. 
    /// Throws an exception if no property was found.
    python::Object property_value(const char* name) const;

    /// Returns the value for the specified property. 
    /// Throws an exception if no property was found.
    template<typename T>
    T property_value(const char* name) const;

    const Guid& node_id() const;
    void set_node_id(const Guid& id);

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);

    const char* node_class() const;
    const char* title() const;
    const char* category() const;

    void add_pin(const char* name, int pin_type);

    /// Increases refcount of pin
    void add_pin(FlowPin* pin);
    
    const std::vector<FlowProperty*>& properties() const;

    const Vec2i& ui_pos() const;
    void set_ui_pos(const Vec2i& pos);

    const char* ui_class() const;
    void set_ui_class(const char* cls);

    void set_ui_node(QtFlowNode* node);
    void invoke_ui_method(const python::Tuple& args);

    FlowNode(const FlowNode&);

    /// Called whenever the given pin receives a new link
    void on_pin_linked(FlowPin* pin);
    /// Called whenever the given pin is unlinked from another pin
    void on_pin_unlinked(FlowPin* pin);

protected:
    /// Adds a new array pin after the specified pin
    void add_array_pin(ArrayFlowPin* prev);

    /// Removes an array pin updates all other pins accordingly
    void remove_array_pin(ArrayFlowPin* pin);

    std::string _node_class;
    std::string _title;
    std::string _category;

    std::vector<FlowPin*> _pins;
    std::vector<FlowProperty*> _properties;

    FlowNodeFunction _function;

    FlowGraph* _owner_graph;
    Guid _node_id;

    std::string _ui_class;
    Vec2i _ui_pos;

    QtFlowNode* _ui_node;
};


template<typename T>
T FlowNode::property_value(const char* name) const
{
    return python::from_python<T>(property_value(name));
}


#endif // __FLOW_NODE_H__
