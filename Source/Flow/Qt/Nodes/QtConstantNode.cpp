#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtConstantNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtConstantNode::QtConstantNode(FlowNode* node, QGraphicsWidget* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->attribute<const char*>("value");
}
QtConstantNode::~QtConstantNode()
{
}
void QtConstantNode::node_updated()
{
    _text = _node->attribute<const char*>("value");
    QtSinglePinNode::node_updated();
}

