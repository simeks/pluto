#include <Core/Common.h>

#include "Flow/FlowNode.h"
#include "QtBaseNode.h"
#include "QtFlowLink.h"
#include "QtFlowPin.h"

#include <QGraphicsGridLayout>
#include <QGraphicsSceneMoveEvent>
#include <QPainter>

QtBaseNode::QtBaseNode(FlowNode* node, QGraphicsItem* parent) : 
    QGraphicsWidget(parent),
    _node(nullptr)
{
    _layout = new QGraphicsGridLayout(this);
    _layout->setContentsMargins(2, 2, 2, 2);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);

    const Vec2i& pos = node->ui_pos();
    setPos(pos.x, pos.y);

    _node = node;
    _node->addref();
}
QtBaseNode::~QtBaseNode()
{
    _node->release();
}
QRectF QtBaseNode::boundingRect() const
{
    return contentsRect();
}
void QtBaseNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setBrush(QBrush(Qt::lightGray));

    if (isSelected())
        painter->setPen(QPen(Qt::black, 2, Qt::DashLine));

    painter->drawRect(contentsRect());
}
FlowNode* QtBaseNode::node() const
{
    return _node;
}
Guid QtBaseNode::node_id() const
{
    if (_node)
        return _node->node_id();
    return Guid();
}

const std::vector<QtFlowPin*>& QtBaseNode::pins() const
{
    return _pins;
}
void QtBaseNode::moveEvent(QGraphicsSceneMoveEvent* e)
{
    if (_node)
    {
        QPointF p = e->newPos();
        _node->set_ui_pos(Vec2i(p.x(), p.y()));
    }
    QGraphicsWidget::moveEvent(e);
}
