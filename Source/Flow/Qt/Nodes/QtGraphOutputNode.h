#ifndef __FLOW_QT_GRAPH_OUTPUT_NODE_H__
#define __FLOW_QT_GRAPH_OUTPUT_NODE_H__

#include "QtSinglePinNode.h"

class QtGraphOutputNode : public QtSinglePinNode
{
public:
    QtGraphOutputNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtGraphOutputNode();

    virtual void node_updated() OVERRIDE;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;
};

#endif // __FLOW_QT_GRAPH_OUTPUT_NODE_H__
