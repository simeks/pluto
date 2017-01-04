#ifndef __FLOW_QT_SINGLE_PIN_NODE_H__
#define __FLOW_QT_SINGLE_PIN_NODE_H__

#include "../QtFlowNode.h"

class QtSinglePinNode : public QtFlowNode
{
public:
    QtSinglePinNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtSinglePinNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    const QString& text() const;

    QtFlowPin* _pin;
    QString _text;
};

#endif // __FLOW_QT_SINGLE_PIN_NODE_H__
