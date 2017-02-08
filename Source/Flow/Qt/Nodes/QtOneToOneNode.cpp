#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtOneToOneNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

using namespace python_convert;

QtOneToOneNode::QtOneToOneNode(FlowNode* node, QGraphicsObject* parent) :
    QtFlowNode(node, parent)
{
    if (_node->has_attribute("ui_node_title_var"))
    {
        const char* var = from_python<const char*>(_node->attribute("ui_node_title_var"));
        _text = from_python<const char*>(_node->attribute(var));
    }
    else
    {
        _text = _node->title();
    }

    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
}
QtOneToOneNode::~QtOneToOneNode()
{
}
void QtOneToOneNode::node_updated()
{
    if (_node->has_attribute("ui_node_title_var"))
    {
        const char* var = from_python<const char*>(_node->attribute("ui_node_title_var"));
        _text = from_python<const char*>(_node->attribute(var));
    }
    else
    {
        _text = _node->title();
    }

    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }

    QtFlowNode::node_updated();

    calculate_size();
    
    for (auto& p : _pins)
    {
        QPointF pin_pos = p->local_pos();
        if (p->pin_type() == FlowPin::Out)
            pin_pos.setX(_rect.width());
        else
            pin_pos.setX(0);

        p->set_local_pos(pin_pos);
    }

    update();
    scene()->update();
}
void QtOneToOneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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

    painter->drawText(title_pos, _text);

    for (QtFlowPin* pin : _pins)
    {
        painter->setBrush(pin->color());
        painter->setPen(pin->outline_color());
        painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);
    }
}
void QtOneToOneNode::create_pins()
{
    assert(_node->pins().size() == 2);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    
    FlowPin* pin = _node->pins()[0];
    QPoint pin_pos;
    if (pin->pin_type() == FlowPin::Out) 
        pin_pos = QPoint(_rect.width(), _rect.height() / 2);
    else
        pin_pos = QPoint(0, _rect.height() / 2);

    _pins.push_back(new QtFlowPin(this, pin, pin_pos));

    pin = _node->pins()[1];
    if (pin->pin_type() == FlowPin::Out)
        pin_pos = QPoint(_rect.width(), _rect.height() / 2);
    else
        pin_pos = QPoint(0, _rect.height() / 2);
    
    _pins.push_back(new QtFlowPin(this, pin, pin_pos));
}
void QtOneToOneNode::calculate_size()
{
    assert(_node->pins().size() == 2);

    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 10; // Title
    int width = std::max(40, font_metrics.width(_text)+20);
    _rect = QRect(0, 0, width, height);
}
