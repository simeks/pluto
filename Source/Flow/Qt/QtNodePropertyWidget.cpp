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

#include <qteditorfactory.h>
#include <qtpropertymanager.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

QtNodePropertyWidget::QtNodePropertyWidget(QWidget *parent) 
    : QWidget(parent),
    _string_property_manager(nullptr),
    _int_property_manager(nullptr),
    _double_property_manager(nullptr),
    _bool_property_manager(nullptr),
    _file_property_manager(nullptr),
    _property_browser(nullptr),
    _selected_node(nullptr)
{
    setup_ui();
}

QtNodePropertyWidget::~QtNodePropertyWidget()
{
}

void QtNodePropertyWidget::setup_ui()
{
    _string_property_manager = new QtStringPropertyManager(this);
    _int_property_manager = new QtIntPropertyManager(this);
    _double_property_manager = new QtDoublePropertyManager(this);
    _bool_property_manager = new QtBoolPropertyManager(this);
    _file_property_manager = new QtFilePropertyManager(this);
    _enum_property_manager = new QtEnumPropertyManager(this);

    QtTreePropertyBrowser* tree_property_browser = new QtTreePropertyBrowser(this);
    _property_browser = tree_property_browser;

    tree_property_browser->setFactoryForManager(_string_property_manager, new QtLineEditFactory(this));
    tree_property_browser->setFactoryForManager(_int_property_manager, new QtSpinBoxFactory(this));
    tree_property_browser->setFactoryForManager(_double_property_manager, new QtDoubleSpinBoxFactory(this));
    tree_property_browser->setFactoryForManager(_bool_property_manager, new QtCheckBoxFactory(this));
    tree_property_browser->setFactoryForManager(_file_property_manager, new QtFileEditorFactory(this));
    tree_property_browser->setFactoryForManager(_enum_property_manager, new QtEnumEditorFactory(this));

    tree_property_browser->setPropertiesWithoutValueMarked(true);
    tree_property_browser->setRootIsDecorated(false);
    tree_property_browser->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);

    QLayout* layout = new QGridLayout(this);
    layout->setMargin(2);
    layout->addWidget(_property_browser);
    setLayout(layout);

    _property_browser->hide();

    connect(_string_property_manager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(property_changed(QtProperty*, const QString&)));
    connect(_int_property_manager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(property_changed(QtProperty*, int)));
    connect(_double_property_manager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(property_changed(QtProperty*, double)));
    connect(_bool_property_manager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(property_changed(QtProperty*, bool)));
    connect(_file_property_manager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(property_changed(QtProperty*, const QString&)));
    connect(_enum_property_manager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(enum_property_changed(QtProperty*, int)));
}
void QtNodePropertyWidget::set_selected(QtFlowNode* selected)
{
    _selected_node = selected;
    if (_selected_node)
    {
        _property_browser->clear();

        FlowNode* node = _selected_node->node();
        for (auto p : node->properties())
        {
            QString property_name = p->name();
            if (p->is_a(FileProperty::static_class()))
            {
                FileProperty* file_prop = object_cast<FileProperty>(p);

                QFileDialog::AcceptMode accept_mode = file_prop->file_mode() == FileProperty::File_Open ? QFileDialog::AcceptOpen : QFileDialog::AcceptSave;

                QtProperty* prop = _file_property_manager->addProperty(property_name, accept_mode, QString::fromStdString(file_prop->file_filter()));
                _file_property_manager->setValue(prop, node->attribute<const char*>(p->name()));
                _property_browser->addProperty(prop);
            }
            else if (p->is_a(BoolProperty::static_class()))
            {
                QtProperty* prop = _bool_property_manager->addProperty(property_name);
                _bool_property_manager->setValue(prop, node->attribute<bool>(p->name()));
                _property_browser->addProperty(prop);
            }
            else if (p->is_a(IntProperty::static_class()))
            {
                QtProperty* prop = _int_property_manager->addProperty(property_name);
                _int_property_manager->setValue(prop, node->attribute<int>(p->name()));
                _property_browser->addProperty(prop);
            }
            else if (p->is_a(FloatProperty::static_class()))
            {
                QtProperty* prop = _double_property_manager->addProperty(property_name);
                _double_property_manager->setValue(prop, node->attribute<double>(p->name()));
                _property_browser->addProperty(prop);
            }
            else if (p->is_a(EnumProperty::static_class()))
            {
                EnumProperty* enum_prop = object_cast<EnumProperty>(p);

                QtProperty* prop = _enum_property_manager->addProperty(property_name);
                QStringList options;
                for (auto& o : enum_prop->options())
                {
                    options.push_back(QString::fromStdString(o));
                }

                _enum_property_manager->setEnumNames(prop, options);
                if (enum_prop->default_index() >= 0)
                    _enum_property_manager->setValue(prop, enum_prop->default_index());

                _property_browser->addProperty(prop);
            }
            else
            {
                QtProperty* prop = _string_property_manager->addProperty(property_name);
                _string_property_manager->setValue(prop, node->attribute<const char*>(p->name()));
                _property_browser->addProperty(prop);
            }
        }
        _property_browser->show();
    }
    else
    {
        _property_browser->clear();
        _property_browser->hide();
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
void QtNodePropertyWidget::property_changed(QtProperty *prop, const QString &val)
{
    if (_selected_node)
    {
        _selected_node->node()->set_property(prop->propertyName().toUtf8().constData(), 
            python::to_python(val.toUtf8().constData()));
        _selected_node->node_updated();
    }
}
void QtNodePropertyWidget::property_changed(QtProperty *prop, int val)
{
    if (_selected_node)
    {
        _selected_node->node()->set_property(prop->propertyName().toUtf8().constData(), 
            python::to_python(val));
        _selected_node->node_updated();
    }
}
void QtNodePropertyWidget::property_changed(QtProperty *prop, double val)
{
    if (_selected_node)
    {
        _selected_node->node()->set_property(prop->propertyName().toUtf8().constData(), 
            python::to_python(val));
        _selected_node->node_updated();
    }
}
void QtNodePropertyWidget::property_changed(QtProperty *prop, bool val)
{
    if (_selected_node)
    {
        _selected_node->node()->set_property(prop->propertyName().toUtf8().constData(), 
            python::to_python(val));
        _selected_node->node_updated();
    }
}
void QtNodePropertyWidget::enum_property_changed(QtProperty* prop, int i)
{
    if (_selected_node)
    {
        QStringList enum_names = _enum_property_manager->enumNames(prop);
        const QString& val = enum_names[i];

        _selected_node->node()->set_property(prop->propertyName().toUtf8().constData(),
            python::to_python(val));

        _selected_node->node_updated();
    }
}

