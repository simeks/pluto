#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtPrintNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtPrintNode::QtPrintNode(FlowNode* node, QGraphicsWidget* parent) :
    QtSinglePinNode(node, parent)
{
    _text = _node->attribute<const char*>("value");
}
QtPrintNode::~QtPrintNode()
{
}
void QtPrintNode::node_finished()
{
    _text = _node->attribute<const char*>("value");
    QtSinglePinNode::node_finished();
}
void QtPrintNode::reset_run_status()
{
    _text = "";
}

