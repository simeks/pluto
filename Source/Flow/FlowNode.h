#ifndef __FLOW_NODE_H__
#define __FLOW_NODE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Vec2.h>

#include "API.h"

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


class FLOW_API FlowNode : public Object
{
    DECLARE_OBJECT(FlowNode, Object);

public:
    struct Property
    {
        enum Flags
        {
            Prop_Public
        };
        std::string name;
        int flags;
    };

    DECLARE_OBJECT_CONSTRUCTOR(FlowNode);
    ~FlowNode();

    void object_init();
    void object_init(const FlowNodeDef& def);
    void object_python_init(const Tuple& args, const Dict& kw);


    void run(FlowContext* ctx);

    const std::vector<FlowPin*>& pins() const;

    FlowPin* pin(int id) const;
    FlowPin* pin(const char* name) const;

    const Guid& node_id() const;
    void set_node_id(const Guid& id);
    void set_flow_graph(FlowGraph* graph);

    const char* node_class() const;
    const char* title() const;
    const char* category() const;

    void add_pin(const char* name, int pin_type);
    void add_pin(FlowPin* pin);

    void add_property(FlowProperty* prop);
    const std::vector<FlowProperty*>& properties() const;

    const char* property(const char* name) const;
    void set_property(const char* name, const char* value);
    
    const Vec2i& ui_pos() const;
    void set_ui_pos(const Vec2i& pos);

    FlowNode(const FlowNode&);
protected:
    std::vector<FlowPin*> _pins;
    std::vector<FlowProperty*> _properties;

    FlowGraph* _owner_graph;
    Guid _node_id;
    Vec2i _ui_pos;

    FlowNodeFunction _function;

};

#endif // __FLOW_NODE_H__
