#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "QtConstantNode.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

using namespace python;

QtConstantNode::QtConstantNode(FlowNode* node, QGraphicsObject* parent) :
    QtSinglePinNode(node, parent)
{
    if (_node->has_attribute("ui_constant_var"))
    {
        const char* var = from_python<const char*>(_node->attribute("ui_constant_var"));
        _text = from_python<const char*>(_node->attribute(var));
    }
    else if (_node->has_attribute("value"))
    {
        PyObject* value = _node->attribute("value");
        _text = python::from_python<const char*>(PyObject_Str(value));
    }
    
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
    if (_node->has_attribute("ui_constant_var"))
    {
        const char* var = from_python<const char*>(_node->attribute("ui_constant_var"));
        _text = from_python<const char*>(_node->attribute(var));
    }
    else if (_node->has_attribute("value"))
    {
        PyObject* value = _node->attribute("value");
        _text = python::from_python<const char*>(PyObject_Str(value));
    }

    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
    QtSinglePinNode::node_updated();
}

