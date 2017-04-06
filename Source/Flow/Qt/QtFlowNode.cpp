#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowGraphScene.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "Style.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMoveEvent>
#include <QPainter>

QtFlowNode::QtFlowNode(FlowNode* node, QGraphicsObject* parent) : QGraphicsObject(parent), _node(nullptr), _highlighted_pin(-1), _status(Idle)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptHoverEvents(true);
    
    const Vec2i& pos = node->ui_pos();
    setPos(pos.x, pos.y);

    _node = node;
    _node->addref();
    /*if (_node->is_a(UiFlowNode::static_class()))
        object_cast<UiFlowNode>(_node)->set_ui_node(this);
*/
    _title = _node->title();
}
QtFlowNode::~QtFlowNode()
{
    for (auto p : _pins)
        delete p;
    _pins.clear();
    _node->release();
}
QRectF QtFlowNode::boundingRect() const
{
    int pin_radius = FlowUIStyle::default_style().pin_radius + FlowUIStyle::default_style().pin_margin;
    return _rect.adjusted(-pin_radius, -pin_radius, pin_radius, pin_radius);
}
void QtFlowNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    fnt.setBold(true);
    QFontMetrics metrics(fnt);

    QRect title_rect = metrics.boundingRect(title());
    QPoint title_pos((_rect.width() - title_rect.width())/2, title_rect.height()+5);

    painter->setFont(fnt);
    painter->setPen(style.node_title_color);
    painter->drawText(title_pos, title());

    paint_status_marker(painter, QPointF(6, 6));

    fnt.setBold(false);
    painter->setFont(fnt);
    paint_pins(painter);
}
void QtFlowNode::paint_status_marker(QPainter* painter, const QPointF& pos)
{
    const FlowUIStyle& style = FlowUIStyle::default_style();

    QColor brush_color;
    QColor pen_color;

    switch (_status)
    {
    case Running:
        brush_color = Qt::yellow;
        pen_color = Qt::darkYellow;
        break;
    case Finished:
        brush_color = Qt::green;
        pen_color = Qt::darkGreen;
        break;
    case Failed:
        brush_color = Qt::red;
        pen_color = Qt::darkRed;
        break;
    //case Idle:
    default:
        brush_color = style.node_background_0;
        pen_color = style.node_background_0;
        break;
    }

    painter->setBrush(brush_color);
    painter->setPen(pen_color);

    painter->drawEllipse(pos, 3, 3);
}
void QtFlowNode::paint_pins(QPainter* painter)
{
    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(painter->font());
    int y_offset = metrics.height() + 25;

    int in_pin = 0, out_pin = 0;
    for (auto& pin : _pins)
    {
        int label_width = metrics.width(pin->pin()->name());

        QPoint text_pos;
        if (pin->pin_type() == FlowPin::In)
        {
            text_pos = QPoint(10, y_offset + in_pin * (metrics.height() + 10));
            ++in_pin;
        }
        else
        {
            text_pos = QPoint(_rect.width() - label_width - 10, y_offset + out_pin * (metrics.height() + 5));
            ++out_pin;
        }

        painter->setPen(style.pin_text_color);

        painter->drawText(text_pos, FlowUIStyle::stylize_text(pin->name()));

        painter->setBrush(pin->color());
        painter->setPen(pin->outline_color());
        painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);
    }
}
int QtFlowNode::check_pin(const QPointF& pt) const
{
    const FlowUIStyle& style = FlowUIStyle::default_style();

    for (int i = 0; i < _pins.size(); ++i)
    {
        QPointF p = pt - _pins[i]->local_pos();
        double distance = std::sqrt(QPointF::dotProduct(p, p));

        if (distance < (style.pin_radius * 2.0 + style.pin_margin))
        {
            return i;
        }
    }
    return -1;
}
const std::vector<QtFlowPin*>& QtFlowNode::pins() const
{
    return _pins;
}
QtFlowPin* QtFlowNode::pin(int idx) const
{
    return _pins[idx];
}
QtFlowPin* QtFlowNode::pin(const char* name) const
{
    for (auto& p : _pins)
    {
        // Case-insensitive comparison
        if (_stricmp(p->name(), name) == 0)
        {
            return p;
        }
    }
    return nullptr;
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
const QString& QtFlowNode::title() const
{
    return _title;
}
QtFlowNode::Status QtFlowNode::status() const
{
    return _status;
}
void QtFlowNode::set_status(Status status)
{
    _status = status;
}

void QtFlowNode::setup()
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setOffset(3, 3);
    effect->setBlurRadius(20);
    effect->setColor(FlowUIStyle::default_style().node_shadow_color);

    setGraphicsEffect(effect);

    calculate_size();
    create_pins();

    update();
}
void QtFlowNode::node_updated()
{
    emit ((QtFlowGraphScene*)scene())->graph_changed();
}
void QtFlowNode::reset_run_status()
{
    _status = Idle;
    update();
}
void QtFlowNode::node_started()
{
    _status = Running;
    update();
}
void QtFlowNode::node_finished()
{
    _status = Finished;
    update();
}
void QtFlowNode::node_failed()
{
    _status = Failed;
    update();
}
void QtFlowNode::move_node(const QPointF& scene_pos)
{
    setPos(scene_pos);
    _node->set_ui_pos(Vec2i(scene_pos.x(), scene_pos.y()));
}


void QtFlowNode::create_pins()
{
    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    int y_offset = metrics.height() + 25;

    int in_pin = 0, out_pin = 0;
    for (auto& pin : _node->pins())
    {
        QPoint pin_pos;
        if (pin->pin_type() == FlowPin::In)
        {
            pin_pos = QPoint(0, y_offset + in_pin * (metrics.height() + 10) - metrics.height() / 2 + 2);
            ++in_pin;
        }
        else
        {
            pin_pos = QPoint(_rect.width(), y_offset + out_pin * (metrics.height() + 10) - metrics.height() / 2 + 2);
            ++out_pin;
        }
        _pins.push_back(new QtFlowPin(this, pin, pin_pos));
    }

}
void QtFlowNode::calculate_size()
{
    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 15; // Title
    
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

    int width = font_metrics.width(title()) + 30;
    for (int i = 0; i < rows; ++i)
    {
        int w = 0;
        if (i < in_pins.size())
            w += font_metrics.width(in_pins[i]->name()) + 20;
        if (i < out_pins.size())
            w += font_metrics.width(out_pins[i]->name()) + 20;

        width = std::max(width, w);
    }
    _rect = QRect(0, 0, width, height);
}
void QtFlowNode::update_pins()
{
    std::vector<QtFlowPin*> new_pins;

    // Find which pins to add
    for (auto& p : _node->pins())
    {
        auto it = std::find_if(_pins.begin(), _pins.end(), [p](QtFlowPin* p2) { return p2->pin() == p; });
        if (it != _pins.end())
        {
            new_pins.push_back(*it);
        }
        else
        {
            new_pins.push_back(new QtFlowPin(this, p, QPointF())); // We update position later
        }
    }

    for (auto& p : _pins)
    {
        if (std::find(new_pins.begin(), new_pins.end(), p) == new_pins.end())
        {
            delete p;
        }
    }

    _pins = new_pins;

    calculate_size();

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    int y_offset = metrics.height() + 25;

    // Update remaining pins
    int in_pin = 0, out_pin = 0;
    for (auto& pin : _pins)
    {
        QPoint pin_pos;
        if (pin->pin_type() == FlowPin::In)
        {
            pin_pos = QPoint(0, y_offset + in_pin * (metrics.height() + 10) - metrics.height() / 2 + 2);
            ++in_pin;
        }
        else
        {
            pin_pos = QPoint(_rect.width(), y_offset + out_pin * (metrics.height() + 10) - metrics.height() / 2 + 2);
            ++out_pin;
        }
        pin->set_local_pos(pin_pos);
    }
    update();
}
int QtFlowNode::type() const
{
    return Type;
}
void QtFlowNode::on_pin_linked(QtFlowPin* pin)
{
    bool need_update = pin->pin()->is_a(ArrayFlowPin::static_class());

    _node->on_pin_linked(pin->pin());

    if (need_update)
        update_pins();
}
void QtFlowNode::on_pin_unlinked(QtFlowPin* pin)
{
    bool need_update = pin->pin()->is_a(ArrayFlowPin::static_class());

    _node->on_pin_unlinked(pin->pin());

    if (need_update)
        update_pins();
}
void QtFlowNode::hoverMoveEvent(QGraphicsSceneHoverEvent* evt)
{
    int pin = check_pin(evt->pos());
    if (pin != _highlighted_pin)
    {
        if (_highlighted_pin != -1)
            _pins[_highlighted_pin]->set_highlight(false);

        _highlighted_pin = pin;
        if (_highlighted_pin != -1)
            _pins[_highlighted_pin]->set_highlight(true);
        update();
    }

    QGraphicsItem::hoverMoveEvent(evt);
}
void QtFlowNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt)
{
    if (_highlighted_pin != -1)
        _pins[_highlighted_pin]->set_highlight(false);
    _highlighted_pin = -1;
    QGraphicsItem::hoverLeaveEvent(evt);
}
