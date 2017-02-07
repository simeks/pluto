#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtConstantNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtConstantNode::QtConstantNode(FlowNode* node, QGraphicsObject* parent) :
    QtSinglePinNode(node, parent)
{
    PyObject* value = _node->attribute("value");
    _text = python_convert::from_python<const char*>(PyObject_Str(value));
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
    PyObject* value = _node->attribute("value");
    _text = python_convert::from_python<const char*>(PyObject_Str(value));
    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
    QtSinglePinNode::node_updated();
}

