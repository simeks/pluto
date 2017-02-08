#ifndef __FLOW_QT_ONE_TO_ONE_NODE_H__
#define __FLOW_QT_ONE_TO_ONE_NODE_H__

#include "../QtFlowNode.h"

class QtOneToOneNode : public QtFlowNode
{
public:
    QtOneToOneNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtOneToOneNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    QString _text;
};

#endif // __FLOW_QT_ONE_TO_ONE_NODE_H__
