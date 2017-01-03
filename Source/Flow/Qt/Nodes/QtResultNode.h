#ifndef __FLOW_QT_RESULT_NODE_H__
#define __FLOW_QT_RESULT_NODE_H__

#include "../QtFlowNode.h"

class QtResultNode : public QtFlowNode
{
public:
    QtResultNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtResultNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    QString value() const;

    QtFlowPin* _out_pin;
};

#endif // __FLOW_QT_RESULT_NODE_H__
