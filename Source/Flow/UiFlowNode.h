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
    PYTHON_OBJECT(UiFlowNode, FlowNode);

public:
    UiFlowNode();
    ~UiFlowNode();

    void set_ui_node(QtFlowNode* node);

    void invoke_ui_method(const Tuple& args);
    
    UiFlowNode(const UiFlowNode&);

protected:
    QtFlowNode* _ui_node;
};


#endif // __UI_FLOW_NODE_H__
