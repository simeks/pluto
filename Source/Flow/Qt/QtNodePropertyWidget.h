#ifndef __QT_NODE_PROPERTY_WIDGET_H__
#define __QT_NODE_PROPERTY_WIDGET_H__

#include <QWidget>

class QLineEdit;
class QtFlowNode;

class QtAbstractPropertyBrowser;
class QtProperty;

class QtBoolPropertyManager;
class QtDoublePropertyManager;
class QtEnumPropertyManager;
class QtFilePropertyManager;
class QtIntPropertyManager;
class QtStringPropertyManager;

class QtNodePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QtNodePropertyWidget(QWidget *parent = nullptr);
    ~QtNodePropertyWidget();

private:
    void setup_ui();
    void set_selected(QtFlowNode* selected);

    QtFlowNode* _selected_node;

    QtStringPropertyManager* _string_property_manager;
    QtIntPropertyManager* _int_property_manager;
    QtDoublePropertyManager* _double_property_manager;
    QtBoolPropertyManager* _bool_property_manager;
    QtFilePropertyManager* _file_property_manager;
    QtEnumPropertyManager* _enum_property_manager;

    QtAbstractPropertyBrowser* _property_browser;
    
public slots:
    void node_selected(QtFlowNode* node);

private slots:
    void property_changed(QtProperty *property, const QString &val);
    void property_changed(QtProperty *property, int val);
    void property_changed(QtProperty *property, double val);
    void property_changed(QtProperty *property, bool val);

    void enum_property_changed(QtProperty* property, int i);
};

#endif // __QT_NODE_PROPERTY_WIDGET_H__
