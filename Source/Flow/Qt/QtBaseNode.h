#ifndef __QT_BASE_NODE_H__
#define __QT_BASE_NODE_H__

#include <QGraphicsWidget>

class FlowNode;
class QGraphicsGridLayout;
class QWidget;
class QtFlowPin;
class QtBaseNode : public QGraphicsWidget
{
public:
    QtBaseNode(FlowNode* node, QGraphicsItem* parent = nullptr);
    virtual ~QtBaseNode();

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    FlowNode* node() const;
    Guid node_id() const;

    const std::vector<QtFlowPin*>& pins() const;

protected:
    void setup();

    virtual void moveEvent(QGraphicsSceneMoveEvent *e) OVERRIDE;

    QGraphicsGridLayout* _layout;
    bool _highlighted;

    std::vector<QtFlowPin*> _pins;

    FlowNode* _node;

};


#endif // __QT_BASE_NODE_H__