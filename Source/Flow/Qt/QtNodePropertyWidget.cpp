#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowProperty.h"
#include "QtFlowNode.h"
#include "QtNodePropertyWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

QtNodePropertyWidget::QtNodePropertyWidget(QWidget *parent) 
    : QWidget(parent),
    _property_grid(nullptr),
    _selected_node(nullptr)
{
    setup_ui();
}

QtNodePropertyWidget::~QtNodePropertyWidget()
{
}

void QtNodePropertyWidget::setup_ui()
{
    setMinimumSize(200, 0);
}
void QtNodePropertyWidget::set_selected(QtFlowNode* selected)
{
    _selected_node = selected;
    if (_selected_node)
    {
        if (_property_grid) delete _property_grid;

        _property_grid = new QWidget(this);
        QGridLayout* layout = new QGridLayout(_property_grid);
        layout->setAlignment(Qt::AlignTop);

        layout->addWidget(new QLabel("<b>Properties</b>"), 0, 0, 1, 2);

        FlowNode* node = _selected_node->node();
        int i = 1;
        for (auto p : node->properties())
        {
            QString property_name = p->name();

            layout->addWidget(new QLabel(property_name), i, 0);
            QLineEdit* edit = new QLineEdit(node->attribute<const char*>(p->name()), this);
            edit->setObjectName(property_name);

            connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(property_changed(const QString&)));
            
            layout->addWidget(edit, i, 1);
            ++i;
        }

        _property_grid->setVisible(true);
    }
    else
    {
        if (_property_grid)
        {
            delete _property_grid;
            _property_grid = nullptr;
        }
    }
}
void QtNodePropertyWidget::flow_node_selected(QtFlowNode* qt_node)
{
    if (qt_node)
    {
        set_selected(qt_node);
    }
    else
    {
        set_selected(nullptr);
    }

}
void QtNodePropertyWidget::property_changed(const QString& txt)
{
    QLineEdit* src = (QLineEdit*)sender();
    if (_selected_node)
    {
        _selected_node->node()->set_property(src->objectName().toUtf8().constData(), txt.toUtf8().constData());
        _selected_node->node_updated();
    }
}
