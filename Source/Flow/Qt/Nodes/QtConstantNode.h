#ifndef __FLOW_QT_CONSTANT_NODE_H__
#define __FLOW_QT_CONSTANT_NODE_H__

#include "QtSinglePinNode.h"

class QtConstantNode : public QtSinglePinNode
{
public:
    QtConstantNode(FlowNode* node, QGraphicsItem* parent = nullptr);
    virtual ~QtConstantNode();

    virtual void node_updated() OVERRIDE;
};

#endif // __FLOW_QT_CONSTANT_NODE_H__
