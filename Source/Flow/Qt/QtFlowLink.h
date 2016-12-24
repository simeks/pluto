#ifndef __QT_FLOW_CONNECTION_H__
#define __QT_FLOW_CONNECTION_H__

#include <QGraphicsPathItem>

class QtFlowPin;
class QtFlowLink : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 2 };

    QtFlowLink(QtFlowPin* start_pin, QtFlowPin* end_pin, QGraphicsItem* parent = nullptr);
    ~QtFlowLink();

    void set_pin(QtFlowPin* pin);
    void unlink();

    QtFlowPin* start_pin() const;
    QtFlowPin* end_pin() const;

    int type() const;

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private:
    void build_path();

    QtFlowPin* _start_pin;
    QtFlowPin* _end_pin;

};

#endif // __QT_FLOW_CONNECTION_H__