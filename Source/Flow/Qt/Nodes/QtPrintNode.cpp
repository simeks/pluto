#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtPrintNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtPrintNode::QtPrintNode(FlowNode* node, QGraphicsWidget* parent) :
    QtFlowNode(node, parent)
{
}
QtPrintNode::~QtPrintNode()
{
}
void QtPrintNode::node_updated()
{
    calculate_size();
    
    QPointF pin_pos = _out_pin->local_pos();
    pin_pos.setX(_rect.width());
    _out_pin->set_local_pos(pin_pos);

    update();
    scene()->update();
}
void QtPrintNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect(boundingRect());
    painter->setRenderHint(QPainter::Antialiasing);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setBrush(QBrush(style.node_background));

    if (isSelected())
        painter->setPen(QPen(style.node_border_selected, 2));
    else
        painter->setPen(QPen(style.node_border, 2));

    painter->drawRect(_rect);

    QFont fnt = style.node_font;
    QFontMetrics metrics(fnt);

    QPoint title_pos(20, _rect.height() - metrics.height() / 2);

    painter->setFont(fnt);
    painter->setPen(style.node_title_color);

    painter->drawText(title_pos, value());

    QtFlowPin* pin = _pins[0];
    painter->setBrush(pin->color());
    painter->setPen(pin->outline_color());
    painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);

}
void QtPrintNode::create_pins()
{
    assert(_node->pins().size() == 1);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    
    QPoint pin_pos = QPoint(0, _rect.height() / 2);
    _out_pin = new QtFlowPin(this, _node->pins()[0], pin_pos);
    _pins.push_back(_out_pin);
}
void QtPrintNode::calculate_size()
{
    assert(_node->pins().size() == 1);

    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 10; // Title
    int width = std::max(75, font_metrics.width(value())+50);
    _rect = QRect(0, 0, width, height);
}
QString QtPrintNode::value() const
{
    QString value = _node->attribute<const char*>("value");
    if (value.isEmpty())
        value = "[None]";
    return value;
}
