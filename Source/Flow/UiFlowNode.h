#ifndef __UI_FLOW_NODE_H__
#define __UI_FLOW_NODE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Vec2.h>

#include "API.h"
#include "FlowNode.h"

class QtFlowNode;

/// Specialized FlowNode class providing tools for interaction with UI.
class FLOW_API UiFlowNode : public FlowNode
{
    DECLARE_OBJECT(UiFlowNode, FlowNode);

public:
    DECLARE_OBJECT_CONSTRUCTOR(UiFlowNode);
    ~UiFlowNode();

    void object_init();
    void object_init(const FlowNodeDef& def);
    void object_python_init(const Tuple& args, const Dict& kw);

    void set_ui_node(QtFlowNode* node);

    void invoke_ui_method(const Tuple& args);
    
    UiFlowNode(const UiFlowNode&);

protected:
    QtFlowNode* _ui_node;
};


#endif // __UI_FLOW_NODE_H__
