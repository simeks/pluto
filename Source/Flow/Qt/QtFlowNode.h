#ifndef __QT_FLOW_NODE_H__
#define __QT_FLOW_NODE_H__

#include <QGraphicsItem>

class FlowNode;
class QtFlowPin;
class QGraphicsGridLayout;
class QWidget;

class QtFlowNode : public QGraphicsItem
{
public:
    enum { Type = UserType + 8 };

    QtFlowNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtFlowNode();

    void setup();
    virtual void node_updated() {}

    void move_node(const QPointF& scene_pos);

    QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

    /// Returns the pin at the specified point, returns -1 if no pin was found
    /// pt : Local coordinates
    int check_pin(const QPointF& pt) const;

    const std::vector<QtFlowPin*>& pins() const;
    QtFlowPin* pin(int idx) const;

    FlowNode* node() const;
    Guid node_id() const;

    int type() const;

protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* evt) OVERRIDE;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) OVERRIDE;

    virtual void create_pins();
    virtual void calculate_size();

    void paint_pins(QPainter* painter);

    FlowNode* _node;

    QRect _rect;
    std::vector<QtFlowPin*> _pins;

    int _highlighted_pin;
};

#endif // __QT_FLOW_NODE_H__