#ifndef __QT_FLOW_NODE_H__
#define __QT_FLOW_NODE_H__

#include "QtBaseNode.h"

class FlowNode;
class QtFlowNode : public QtBaseNode
{
public:
    enum { Type = UserType + 8 };

    QtFlowNode(FlowNode* node, QGraphicsItem* parent = nullptr);
    virtual ~QtFlowNode();

    int type() const;

private:
    void setup();

};

#endif // __QT_FLOW_NODE_H__