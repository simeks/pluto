#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtConstantNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtConstantNode::QtConstantNode(FlowNode* node, QGraphicsItem* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->attribute<const char*>("value");
    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
}
QtConstantNode::~QtConstantNode()
{
}
void QtConstantNode::node_updated()
{
    _text = _node->attribute<const char*>("value");
    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
    QtSinglePinNode::node_updated();
}

