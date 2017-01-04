#ifndef __FLOW_QT_VARIABLE_NODE_H__
#define __FLOW_QT_VARIABLE_NODE_H__

#include "../QtFlowNode.h"

class QtVariableNode : public QtFlowNode
{
public:
    QtVariableNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtVariableNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    const QString& name() const;

    QtFlowPin* _out_pin;
    QString _name;
};

#endif // __FLOW_QT_VARIABLE_NODE_H__
