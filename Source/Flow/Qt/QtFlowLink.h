#ifndef __QT_FLOW_CONNECTION_H__
#define __QT_FLOW_CONNECTION_H__

#include <QGraphicsPathItem>

class QtFlowPin;
class QtFlowLink : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 2 };

    QtFlowLink(QtFlowPin* start, QtFlowPin* end, QGraphicsItem* parent = nullptr);
    ~QtFlowLink();

    void set_pin(QtFlowPin* pin);
    void unset_pin(int pin_type);

    /// pos : in scene coordinates
    void move_free_end(const QPointF& pos);

    QtFlowPin* start() const;
    QtFlowPin* end() const;

    int type() const;

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    
private:
    void build_path();

    QtFlowPin* _start;
    QtFlowPin* _end;

    QPointF _free_end;

};

#endif // __QT_FLOW_CONNECTION_H__