#include <Core/Common.h>
#include "FlowNode.h"

#include "QtFlowLink.h"
#include "QtFlowPin.h"
#include "QtTerminalNode.h"

#include <QGraphicsGridLayout>
#include <QLabel>

QtTerminalNode::QtTerminalNode(FlowNode* node, QGraphicsItem* parent) : QtBaseNode(node, parent)
{
    _layout->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setup();
}
QtTerminalNode::~QtTerminalNode()
{
}
void QtTerminalNode::setup()
{
    if (!_node)
        return;

    if (_node->pins().size() == 1)
    {
        FlowPin* pin = _node->pins()[0];
        if (pin->pin_type() == FlowPin::Out)
        {
            QGraphicsProxyWidget* name_label = new QGraphicsProxyWidget(this);
            _name_label = new QLabel();
            _name_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            name_label->setWidget(_name_label);
            name_label->widget()->setContentsMargins(5, 2, 5, 2);
            name_label->widget()->setAttribute(Qt::WA_TranslucentBackground);
            _layout->addItem(name_label, 0, 0, 1, 3, Qt::AlignLeft | Qt::AlignTop);

            QtFlowPin* pin_icon = new QtFlowPin(pin, this);
            pin_icon->setMaximumWidth(15);
            _pins.push_back(pin_icon);

            _layout->addItem(pin_icon, 0, 3, Qt::AlignRight | Qt::AlignTop);
        }
    }
}
//
//const std::string& QtTerminalNode::var_name() const
//{
//    TerminalNode* terminal_node = object_cast<TerminalNode>(node().get());
//    assert(terminal_node);
//    return terminal_node->var_name();
//}
//void QtTerminalNode::set_var_name(const std::string& name)
//{
//    TerminalNode* terminal_node = object_cast<TerminalNode>(node().get());
//    assert(terminal_node);
//    terminal_node->set_var_name(name);
//
//    update_label();
//}
//std::string QtTerminalNode::value_str() const
//{
//    TerminalNode* terminal_node = object_cast<TerminalNode>(node().get());
//    assert(terminal_node);
//
//    std::stringstream ss;
//    if (terminal_node->value()->is_a(FlowString::static_class()))
//    {
//        ss << object_cast<FlowString>(terminal_node->value())->get();
//    }
//    else if (terminal_node->value()->is_a(NumericObject::static_class()))
//    {
//        NumericObject* num = object_cast<NumericObject>(terminal_node->value());
//        if (num->is_integer())
//        {
//            ss << num->as_int();
//        }
//        else
//        {
//            ss << num->as_float();
//        }
//    }
//    else
//    {
//        ss << "[Unknown]";
//    }
//
//    return ss.str();
//}
//void QtTerminalNode::set_value(const std::string& value)
//{
//    TerminalNode* terminal_node = object_cast<TerminalNode>(node().get());
//    assert(terminal_node);
//
//    if (terminal_node->value()->is_a(FlowString::static_class()))
//    {
//        object_cast<FlowString>(terminal_node->value())->set(value);
//    }
//    else if (terminal_node->value()->is_a(NumericObject::static_class()))
//    {
//        NumericObject* num = object_cast<NumericObject>(terminal_node->value());
//        if (value == "")
//        {
//            num->set_int(0);
//        }
//        else
//        {
//            if (num->is_integer())
//            {
//                num->set_int(std::stoi(value));
//            }
//            else
//            {
//                num->set_float(std::stof(value));
//            }
//        }
//    }
//}
int QtTerminalNode::type() const
{
    return Type;
}
//void QtTerminalNode::update_label()
//{
//    QString node_label_txt = QString("%1").arg(QString::fromStdString(var_name()));
//    int width = _name_label->fontMetrics().width(node_label_txt);
//    _name_label->setMinimumSize(width, 0);
//    _name_label->setText(node_label_txt);
//}

