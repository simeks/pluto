#ifndef __FLOW_QT_VARIABLE_NODE_H__
#define __FLOW_QT_VARIABLE_NODE_H__

#include "QtSinglePinNode.h"

class QtVariableNode : public QtSinglePinNode
{
public:
    QtVariableNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtVariableNode();

    virtual void node_updated() OVERRIDE;
};

#endif // __FLOW_QT_VARIABLE_NODE_H__
