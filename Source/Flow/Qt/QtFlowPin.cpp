#include <Core/Common.h>

#include "FlowPin.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "Style.h"

#include <QPainter>


QtFlowPin::QtFlowPin(QtFlowNode* owner, FlowPin* pin, const QPointF& local_pos) :
    _owner(owner),
    _pin(pin),
    _local_pos(local_pos),
    _highlighted(false)
{
}
QtFlowPin::~QtFlowPin()
{
}
void QtFlowPin::set_highlight(bool h)
{
    _highlighted = h;
    _owner->update();
}
void QtFlowPin::set_local_pos(const QPointF& pos)
{
    _local_pos = pos;
}
QPointF QtFlowPin::local_pos() const
{
    return _local_pos;
}
QPointF QtFlowPin::pos() const
{
    return _owner->mapToScene(_local_pos);
}
bool QtFlowPin::is_linked() const
{
    return !_pin->links().empty();
}
QtFlowNode* QtFlowPin::owner() const
{
    return _owner;
}
const char* QtFlowPin::name() const
{
    return _pin->name();
}
int QtFlowPin::id() const
{
    return _pin->pin_id();
}
FlowPin* QtFlowPin::pin() const
{
    return _pin;
}
int QtFlowPin::pin_type() const
{
    return _pin->pin_type();
}
QColor QtFlowPin::color() const
{
    if (_highlighted)
        return FlowUIStyle::default_style().pin_color_highlight;
    else
        return FlowUIStyle::default_style().pin_color;
}
QColor QtFlowPin::outline_color() const
{
    if (_highlighted)
        return FlowUIStyle::default_style().pin_outline_color_highlight;
    else
        return FlowUIStyle::default_style().pin_outline_color;
}
