#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtGraphOutputNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtGraphOutputNode::QtGraphOutputNode(FlowNode* node, QGraphicsObject* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->attribute<const char*>("name");
}
QtGraphOutputNode::~QtGraphOutputNode()
{
}
void QtGraphOutputNode::node_updated()
{
    _text = _node->attribute<const char*>("name");

    QtSinglePinNode::node_updated();
}
void QtGraphOutputNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect(boundingRect());
    painter->setRenderHint(QPainter::Antialiasing);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setBrush(QBrush(style.node_background_1));

    if (isSelected())
        painter->setPen(QPen(style.node_border_selected_1, 1));
    else
        painter->setPen(QPen(style.node_border_1, 1));

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

