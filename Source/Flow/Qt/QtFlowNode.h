#ifndef __QT_FLOW_NODE_H__
#define __QT_FLOW_NODE_H__

#include <Core/Platform/Guid.h>
#include <Flow/API.h>

#include <QGraphicsObject>

class FlowNode;
class QtFlowPin;
class QGraphicsGridLayout;
class QWidget;

class FLOW_API QtFlowNode : public QGraphicsObject
{
public:
    enum Status
    {
        Idle,
        Running,
        Finished,
        Failed
    };

    enum { Type = UserType + 1 };

    QtFlowNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtFlowNode();

    void setup();
    virtual void node_updated();

    virtual void reset_run_status();
    virtual void node_started();
    virtual void node_finished();
    virtual void node_failed();

    void move_node(const QPointF& scene_pos);

    QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

    /// Returns the pin at the specified point, returns -1 if no pin was found
    /// pt : Local coordinates
    int check_pin(const QPointF& pt) const;

    const std::vector<QtFlowPin*>& pins() const;
    QtFlowPin* pin(int idx) const;
    QtFlowPin* pin(const char* name) const;

    FlowNode* node() const;
    Guid node_id() const;

    const QString& title() const;

    Status status() const;
    void set_status(Status status);

    int type() const;

    void on_pin_linked(QtFlowPin* pin);
    void on_pin_unlinked(QtFlowPin* pin);

protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* evt) OVERRIDE;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) OVERRIDE;

    virtual void create_pins();
    virtual void calculate_size();
    void update_pins();

    void paint_status_marker(QPainter* painter, const QPointF& pos);
    void paint_pins(QPainter* painter);

    FlowNode* _node;
    QString _title;

    QRect _rect;
    std::vector<QtFlowPin*> _pins;

    int _highlighted_pin;

    // Run status
    Status _status;

};

#endif // __QT_FLOW_NODE_H__