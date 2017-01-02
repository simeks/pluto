#ifndef __QT_FLOW_NODE_H__
#define __QT_FLOW_NODE_H__

#include <QGraphicsItem>

class FlowNode;
class QGraphicsGridLayout;
class QWidget;
class QtFlowNode : public QGraphicsItem
{
public:
    enum { Type = UserType + 8 };

    QtFlowNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtFlowNode();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    FlowNode* node() const;
    Guid node_id() const;

    int type() const;

private:
    virtual QVariant QtFlowNode::itemChange(GraphicsItemChange change, const QVariant &value) OVERRIDE;
    
    void setup();
    void calculate_size();

    void paint_pins(QPainter* painter);

    bool _highlighted;

    FlowNode* _node;

    QRect _rect;
};

#endif // __QT_FLOW_NODE_H__