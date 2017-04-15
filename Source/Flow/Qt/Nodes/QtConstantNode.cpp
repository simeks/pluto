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
    if (_node->has_attribute("ui_constant_var"))
    {
        const char* var = _node->property_value<const char*>("ui_constant_var");
        _text = _node->attribute<const char*>(var);
    }
    else if (_node->has_attribute("value"))
    {
        python::Object value = _node->property_value("value");
        _text = python::from_python<const char*>(PyObject_Str(value.ptr()));
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
        const char* var = _node->property_value<const char*>("ui_constant_var");
        _text = _node->attribute<const char*>(var);
    }
    else if (_node->has_attribute("value"))
    {
        python::Object value = _node->property_value("value");
        _text = python::from_python<const char*>(PyObject_Str(value.ptr()));
    }

    if (_text.length() > 50)
    {
        _text = _text.right(50);
        _text += "...";
    }
    QtSinglePinNode::node_updated();
}

