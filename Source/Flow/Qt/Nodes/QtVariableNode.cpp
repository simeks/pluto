#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtVariableNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtVariableNode::QtVariableNode(FlowNode* node, QGraphicsObject* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->property_value<const char*>("name");
}
QtVariableNode::~QtVariableNode()
{
}
void QtVariableNode::node_updated()
{
    _text = _node->property_value<const char*>("name");
    QtSinglePinNode::node_updated();
}
