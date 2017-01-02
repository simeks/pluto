#ifndef __QT_FLOW_CONNECTION_H__
#define __QT_FLOW_CONNECTION_H__

#include <QGraphicsPathItem>

class QtFlowLink : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 2 };

    QtFlowLink(const QPoint& start_pos, const QPoint& end_pos, QGraphicsItem* parent = nullptr);
    ~QtFlowLink();

    void set_pin(FlowPin* pin);
    
    FlowPin* start_pin() const;
    FlowPin* end_pin() const;

    int type() const;

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    
private:
    void build_path();

    FlowPin* _start_pin;
    FlowPin* _end_pin;

    QPoint _start_pos;
    QPoint _end_pos;
};

#endif // __QT_FLOW_CONNECTION_H__