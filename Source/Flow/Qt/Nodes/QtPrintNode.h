#ifndef __FLOW_QT_PRINT_NODE_H__
#define __FLOW_QT_PRINT_NODE_H__

#include "../QtFlowNode.h"

class QtPrintNode : public QtFlowNode
{
public:
    QtPrintNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtPrintNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    const QString& value() const;

    QtFlowPin* _in_pin;
    QString _value;
};

#endif // __FLOW_QT_PRINT_NODE_H__
