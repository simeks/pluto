#include <Core/Common.h>

#include "FlowPin.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "Style.h"

#include <QPainter>

QtFlowLink::QtFlowLink(QtFlowPin* start, QtFlowPin* end, QGraphicsItem* parent)
    : QGraphicsPathItem(parent),
    _start(start),
    _end(end)
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
        _start = pin;
    }
    else
    {
        _end = pin;
    }
}
void QtFlowLink::unset_pin(int pin_type)
{
    if (pin_type == FlowPin::Out)
    {
        _start = nullptr;
    }
    else
    {
        _end = nullptr;
    }
}
void QtFlowLink::move_free_end(const QPointF& pos)
{
    _free_end = pos;
}

QtFlowPin* QtFlowLink::start() const
{
    return _start;
}
QtFlowPin* QtFlowLink::end() const
{
    return _end;
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

    painter->setRenderHint(QPainter::Antialiasing);

    build_path();

    const FlowUIStyle& style = FlowUIStyle::default_style();

    if (isSelected())
    {
        painter->setPen(QPen(style.link_color_selected, 3));
    }
    else if (!_start || !_end)
    {
        painter->setPen(QPen(style.link_color, 2, Qt::DotLine));
    }
    else
    {
        painter->setPen(QPen(style.link_color, 2));
    }
    painter->setBrush(Qt::NoBrush);

    QPainterPath p = path();
    painter->drawPath(p);

    float arrow_start_percent = p.percentAtLength(p.length() - 10);
    float arrow_end_percent = p.percentAtLength(p.length());

    QPointF start_p = p.pointAtPercent(arrow_start_percent);
    QPointF end_p = p.pointAtPercent(arrow_end_percent);
    QLineF arrow_line(start_p, end_p);

    QPointF norm_head(arrow_line.dy(), -arrow_line.dx());
    QPointF arrow_p1 = start_p + norm_head * 0.4;
    QPointF arrow_p2 = start_p - norm_head * 0.4;

    QPolygonF arrow_head_end;
    arrow_head_end << end_p << arrow_p1 << arrow_p2 /*<< headEndP*/;

    //do this after creating other arrow, in case it matters in arrow calc on path
    p.addPolygon(arrow_head_end);

    //fill
    if (isSelected())
    {
        painter->setBrush(style.link_color_selected);
        painter->setPen(QPen(style.link_color_selected, 3));
    }
    else if (!_start || !_end)
    {
        painter->setBrush(style.link_color);
        painter->setPen(QPen(style.link_color, 2));
    }
    else
    {
        painter->setBrush(style.link_color);
        painter->setPen(QPen(style.link_color, 2));
    }

    painter->drawPolygon(arrow_head_end);
}
void QtFlowLink::build_path()
{
    QPointF start, end;
    if (_start)
        start = _start->pos();
    else
        start = _free_end;

    if (_end)
        end = _end->pos();
    else
        end = _free_end;

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
