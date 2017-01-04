#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtVariableNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtVariableNode::QtVariableNode(FlowNode* node, QGraphicsWidget* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->attribute<const char*>("name");
}
QtVariableNode::~QtVariableNode()
{
}
void QtVariableNode::node_updated()
{
    _text = _node->attribute<const char*>("name");
    QtSinglePinNode::node_updated();
}
