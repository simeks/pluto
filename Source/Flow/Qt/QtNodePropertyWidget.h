#ifndef __QT_NODE_PROPERTY_WIDGET_H__
#define __QT_NODE_PROPERTY_WIDGET_H__

#include <QWidget>

class QLineEdit;
class QtFlowNode;
class QtTerminalNode;
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

    QWidget* _property_grid;
    
public slots:
    void flow_node_selected(QtFlowNode* node);

private slots:
    void property_changed(const QString& txt);
};

#endif // __QT_NODE_PROPERTY_WIDGET_H__
