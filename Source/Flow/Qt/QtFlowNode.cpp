#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "Style.h"

#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMoveEvent>
#include <QLabel>
#include <QPainter>


QtFlowNode::QtFlowNode(FlowNode* node, QGraphicsWidget* parent) : QGraphicsItem(parent), _node(nullptr)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);

    const Vec2i& pos = node->ui_pos();
    setPos(pos.x, pos.y);

    _node = node;
    _node->addref();

    setup();
}
QtFlowNode::~QtFlowNode()
{
    _node->release();
}
QRectF QtFlowNode::boundingRect() const
{
    int pin_radius = FlowUIStyle::default_style().pin_radius;
    return _rect.adjusted(-pin_radius, -pin_radius, pin_radius, pin_radius);
}
void QtFlowNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    fnt.setBold(true);
    QFontMetrics metrics(fnt);

    QRect title_rect = metrics.boundingRect(_node->title());

    QPoint title_pos((_rect.width() - title_rect.width())/2, title_rect.height()+5);

    painter->setFont(fnt);
    painter->setPen(style.node_title_color);
    painter->drawText(title_pos, _node->title());

    fnt.setBold(false);
    painter->setFont(fnt);
    paint_pins(painter);
}
void QtFlowNode::paint_pins(QPainter* painter)
{
    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(painter->font());
    int y_offset = metrics.height() + 25;

    int in_pin = 0, out_pin = 0;
    for (auto& pin : _node->pins())
    {
        /*QtFlowPin* pin_icon = new QtFlowPin(pin, this);
        pin_icon->setMaximumWidth(15);
        _pins.push_back(pin_icon);
*/
        int label_width = metrics.width(QString::fromStdString(pin->name()));

        QPoint text_pos;
        QPoint pin_pos;
        if (pin->pin_type() == FlowPin::In)
        {
            text_pos = QPoint(10, y_offset + in_pin * (metrics.height() + 10));
            pin_pos = QPoint(0, y_offset + in_pin * (metrics.height() + 10) - metrics.height()/2 + 2);
            ++in_pin;
        }
        else
        {
            text_pos = QPoint(_rect.width() - label_width - 10, y_offset + out_pin * (metrics.height() + 10));
            pin_pos = QPoint(_rect.width(), y_offset + out_pin * (metrics.height() + 10) - metrics.height() / 2 + 2);
            ++out_pin;
        }

        painter->setPen(style.pin_text_color);
        painter->drawText(text_pos, QString::fromStdString(pin->name()));

        painter->setBrush(Qt::white);
        painter->setPen(Qt::gray);
        painter->drawEllipse(pin_pos, style.pin_radius, style.pin_radius);
        painter->setBrush(Qt::NoBrush);
    }
}
FlowNode* QtFlowNode::node() const
{
    return _node;
}
Guid QtFlowNode::node_id() const
{
    if (_node)
        return _node->node_id();
    return Guid();
}

void QtFlowNode::setup()
{
    calculate_size();

    update();
}
void QtFlowNode::calculate_size()
{
    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 20; // Title
    
    std::vector<FlowPin*> in_pins, out_pins;
    for (auto& pin : _node->pins())
    {
        if (pin->pin_type() == FlowPin::In)
            in_pins.push_back(pin);
        else
            out_pins.push_back(pin);

    }
    int rows = std::max((int)in_pins.size(), (int)out_pins.size());
    height +=  rows * (font_metrics.height() + 10); // Pins

    int width = font_metrics.width(_node->title()) + 20;
    for (int i = 0; i < rows; ++i)
    {
        int w = 0;
        if (i < in_pins.size())
            w += font_metrics.width(QString::fromStdString(in_pins[i]->name())) + 20;
        if (i < out_pins.size())
            w += font_metrics.width(QString::fromStdString(out_pins[i]->name())) + 20;

        width = std::max(width, w);
    }
    _rect = QRect(0, 0, width, height);
}

int QtFlowNode::type() const
{
    return Type;
}
QVariant QtFlowNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && _node) 
    {
        QPoint p = value.toPoint();
        _node->set_ui_pos(Vec2i(p.x(), p.y()));
    }
    return QGraphicsItem::itemChange(change, value);
}
