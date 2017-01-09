#ifndef __FLOW_QT_GRAPH_INPUT_NODE_H__
#define __FLOW_QT_GRAPH_INPUT_NODE_H__

#include "QtSinglePinNode.h"

class QtGraphInputNode : public QtSinglePinNode
{
public:
    QtGraphInputNode(FlowNode* node, QGraphicsItem* parent = nullptr);
    virtual ~QtGraphInputNode();

    virtual void node_updated() OVERRIDE;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;
};

#endif // __FLOW_QT_GRAPH_INPUT_NODE_H__
