//#ifndef __QT_NODE_PROPERTY_WIDGET_H__
//#define __QT_NODE_PROPERTY_WIDGET_H__
//
//#include <QDockWidget>
//
//class QLineEdit;
//class QtBaseNode;
//class QtTerminalNode;
//class QtNodePropertyWidget : public QDockWidget
//{
//    Q_OBJECT
//
//public:
//    explicit QtNodePropertyWidget(QWidget *parent = nullptr);
//    ~QtNodePropertyWidget();
//
//private:
//    void setup_ui();
//    void set_selected(QtTerminalNode* selected);
//
//    QtTerminalNode* _selected_node;
//
//    QWidget* _terminal_property_widget;
//    QLineEdit* _name_edit;
//    QLineEdit* _value_edit;
//
//public slots:
//    void flow_node_selected(QtBaseNode* node);
//
//private slots:
//    void on_name_changed(const QString& txt);
//    void on_value_changed(const QString& txt);
//};
//
//#endif // __QT_NODE_PROPERTY_WIDGET_H__
