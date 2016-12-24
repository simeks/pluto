#include <Core/Common.h>

#include "QtFlowLink.h"
#include "QtFlowPin.h"

#include <QPainter>

QtFlowLink::QtFlowLink(QtFlowPin* start_pin, QtFlowPin* end_pin, QGraphicsItem* parent)
: QGraphicsPathItem(parent),
  _start_pin(start_pin),
  _end_pin(end_pin)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(-1);

    build_path();
}
QtFlowLink::~QtFlowLink()
{
}

void QtFlowLink::set_pin(QtFlowPin* pin)
{
    if (pin->pin_type() == FlowPin::Out)
    {
        _start_pin = pin;
    }
    else
    {
        _end_pin = pin;
    }
}

QtFlowPin* QtFlowLink::start_pin() const
{
    return _start_pin;
}
QtFlowPin* QtFlowLink::end_pin() const
{
    return _end_pin;
}
int QtFlowLink::type() const
{
    return Type;
}

QRectF QtFlowLink::boundingRect() const
{
    qreal extra = 3;
    return QGraphicsPathItem::boundingRect().normalized().adjusted(-extra, -extra, extra, extra);
}
void QtFlowLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!_start_pin || !_end_pin)
    {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    build_path();

    painter->setPen(QPen(Qt::black, 2));
    //line
    //painter->setBrush(mColor);
    painter->setBrush(Qt::NoBrush);

    if (isSelected())
        painter->setPen(QPen(Qt::black, 2, Qt::DashLine));

    QPainterPath p = path();
    painter->drawPath(p);

    //fill
    painter->setBrush(Qt::black);

    float arrow_start_percent = p.percentAtLength(p.length() - 20);
    float arrow_end_percent = p.percentAtLength(p.length()-10);


    QPointF start_P = p.pointAtPercent(arrow_start_percent);
    QPointF end_p = p.pointAtPercent(arrow_end_percent);
    QLineF arrow_line(start_P, end_p);

    QPointF norm_head(arrow_line.dy(), -arrow_line.dx());
    QPointF arrow_p1 = start_P + norm_head * 0.4;
    QPointF arrow_p2 = start_P - norm_head * 0.4;

    QPolygonF arrow_head_end;
    arrow_head_end << end_p << arrow_p1 << arrow_p2 /*<< headEndP*/;

    //do this after creating other arrow, in case it matters in arrow calc on path
    p.addPolygon(arrow_head_end);
    painter->drawPolygon(arrow_head_end);
}
void QtFlowLink::build_path()
{
    QPointF start(_start_pin->scenePos() + QPointF(7, 7));
    QPointF end(_end_pin->scenePos() + QPointF(7, 7));

    qreal dx = fabs(end.x() - start.x());

    QPointF cp1 = start;
    QPointF cp2 = end;

    qreal move_x = 0.45 * dx;

    cp1.setX(cp1.x() + move_x);
    cp2.setX(cp2.x() - move_x);


    QPainterPath p(start);
    p.cubicTo(cp1, cp2, end);
    setPath(p);
}
