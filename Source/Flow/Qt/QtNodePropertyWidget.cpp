//#include <Core/Common.h>
//
//#include <Core/Flow/FlowNode.h>
//#include <Core/Flow/TerminalNode.h>
//
//#include "QtBaseNode.h"
//#include "QtNodePropertyWidget.h"
//#include "QtTerminalNode.h"
//
//#include <QLabel>
//#include <QLineEdit>
//#include <QStandardItemModel>
//#include <QTreeView>
//#include <QVBoxLayout>
//
//QtNodePropertyWidget::QtNodePropertyWidget(QWidget *parent) 
//    : QDockWidget(parent),
//    _terminal_property_widget(nullptr)
//{
//    setup_ui();
//}
//
//QtNodePropertyWidget::~QtNodePropertyWidget()
//{
//}
//
//void QtNodePropertyWidget::setup_ui()
//{
//    setMinimumSize(200, 0);
//
//    _terminal_property_widget = new QWidget(this);
//    setWidget(_terminal_property_widget);
//
//    QGridLayout* layout = new QGridLayout(_terminal_property_widget);
//    _terminal_property_widget->setLayout(layout);
//    layout->setAlignment(Qt::AlignTop);
//
//    layout->addWidget(new QLabel("<b>Properties</b>", _terminal_property_widget), 0, 0, 1, 2);
//    layout->addWidget(new QLabel("Name: ", _terminal_property_widget), 1, 0);
//    
//    _name_edit = new QLineEdit(_terminal_property_widget);
//    connect(_name_edit, SIGNAL(textChanged(const QString &)), this, SLOT(on_name_changed(const QString &)));
//    layout->addWidget(_name_edit, 1, 1);
//
//    layout->addWidget(new QLabel("Value: ", _terminal_property_widget), 2, 0);
//    _value_edit = new QLineEdit(_terminal_property_widget);
//    connect(_value_edit, SIGNAL(textChanged(const QString &)), this, SLOT(on_value_changed(const QString &)));
//    layout->addWidget(_value_edit, 2, 1);
//
//    _terminal_property_widget->setVisible(false);
//}
//void QtNodePropertyWidget::set_selected(QtTerminalNode* selected)
//{
//    _selected_node = selected;
//    if (_selected_node)
//    {
//        _name_edit->setText(QString::fromStdString(_selected_node->var_name()));
//        _value_edit->setText(QString::fromStdString(_selected_node->value_str()));
//        _terminal_property_widget->setVisible(true);
//    }
//    else
//    {
//        _terminal_property_widget->setVisible(false);
//    }
//}
//void QtNodePropertyWidget::flow_node_selected(QtBaseNode* qt_node)
//{
//    if (qt_node)
//    {
//        if (qt_node->type() == QtTerminalNode::Type)
//        {
//            set_selected((QtTerminalNode*)qt_node);
//        }
//        else
//        {
//            set_selected(nullptr);
//        }
//    }
//    else
//    {
//        set_selected(nullptr);
//    }
//
//}
//void QtNodePropertyWidget::on_name_changed(const QString& txt)
//{
//    if (_selected_node)
//    {
//        _selected_node->set_var_name(txt.toStdString());
//    }
//}
//void QtNodePropertyWidget::on_value_changed(const QString& txt)
//{
//    if (_selected_node)
//    {
//        if (txt.length() == 0 || txt[txt.length() - 1] != '.')
//        {
//            _selected_node->set_value(txt.toStdString());
//            _value_edit->setText(QString::fromStdString(_selected_node->value_str()));
//        }
//    }
//}
