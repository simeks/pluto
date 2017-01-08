#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtSinglePinNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtSinglePinNode::QtSinglePinNode(FlowNode* node, QGraphicsWidget* parent) :
    QtFlowNode(node, parent)
{
    _text = "";
}
QtSinglePinNode::~QtSinglePinNode()
{
}
void QtSinglePinNode::node_updated()
{
    QtFlowNode::node_updated();

    calculate_size();
    
    QPointF pin_pos = _pin->local_pos();
    if (_pin->pin_type() == FlowPin::Out)
        pin_pos.setX(_rect.width());
    else
        pin_pos.setX(0);

    _pin->set_local_pos(pin_pos);

    update();
    scene()->update();
}
void QtSinglePinNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect(boundingRect());
    painter->setRenderHint(QPainter::Antialiasing);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setBrush(QBrush(style.node_background_0));

    if (isSelected())
        painter->setPen(QPen(style.node_border_selected_0, 1));
    else
        painter->setPen(QPen(style.node_border_0, 1));

    painter->drawRect(_rect);

    QFont fnt = style.node_font;
    QFontMetrics metrics(fnt);

    QPoint title_pos(10, _rect.height() - metrics.height() / 2);

    painter->setFont(fnt);
    painter->setPen(style.node_title_color);

    painter->drawText(title_pos, text());

    QtFlowPin* pin = _pins[0];
    painter->setBrush(pin->color());
    painter->setPen(pin->outline_color());
    painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);

}
void QtSinglePinNode::create_pins()
{
    assert(_node->pins().size() == 1);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    
    FlowPin* pin = _node->pins()[0];
    QPoint pin_pos;
    if (pin->pin_type() == FlowPin::Out) 
        pin_pos = QPoint(_rect.width(), _rect.height() / 2);
    else
        pin_pos = QPoint(0, _rect.height() / 2);
    _pin = new QtFlowPin(this, _node->pins()[0], pin_pos);
    _pins.push_back(_pin);
}
void QtSinglePinNode::calculate_size()
{
    assert(_node->pins().size() == 1);

    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 10; // Title
    int width = std::max(40, font_metrics.width(text())+25);
    _rect = QRect(0, 0, width, height);
}
const QString& QtSinglePinNode::text() const
{
    return _text;
}
