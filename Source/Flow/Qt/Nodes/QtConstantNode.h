#ifndef __FLOW_QT_CONSTANT_NODE_H__
#define __FLOW_QT_CONSTANT_NODE_H__

#include "../QtFlowNode.h"

class QtConstantNode : public QtFlowNode
{
public:
    QtConstantNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtConstantNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    const QString& value() const;

    QtFlowPin* _out_pin;
    QString _value;
};

#endif // __FLOW_QT_CONSTANT_NODE_H__
