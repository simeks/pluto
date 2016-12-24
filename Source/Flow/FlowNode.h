#ifndef __FLOW_NODE_H__
#define __FLOW_NODE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Vec2.h>

#include "API.h"

class FlowContext;
class FlowGraph;
class FlowPin;

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

    FlowNode();
    explicit FlowNode(const FlowNodeDef& def);
    ~FlowNode();

    void run(FlowContext* ctx);

    const std::vector<FlowPin*>& pins() const;

    FlowPin* pin(int id) const;
    FlowPin* pin(const std::string& name) const;

    const Guid& node_id() const;
    void set_node_id(const Guid& id);
    void set_flow_graph(FlowGraph* graph);

    FlowNode(const FlowNode&);
    FlowNode& operator=(const FlowNode&);

    const char* node_class() const;
    const char* title() const;
    const char* category() const;

    void add_pin(const std::string& name, int pin_type);
    void add_property(const std::string& name, int flags, PyObject* value);

    const Vec2i& ui_pos() const;
    void set_ui_pos(const Vec2i& pos);

protected:
    std::vector<FlowPin*> _pins;

    FlowGraph* _owner_graph;
    Guid _node_id;
    Vec2i _ui_pos;

    FlowNodeFunction _function;
};

#endif // __FLOW_NODE_H__
