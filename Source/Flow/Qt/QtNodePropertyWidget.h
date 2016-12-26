#ifndef __QT_NODE_PROPERTY_WIDGET_H__
#define __QT_NODE_PROPERTY_WIDGET_H__

#include <QWidget>

class QLineEdit;
class QtBaseNode;
class QtTerminalNode;
class QtNodePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QtNodePropertyWidget(QWidget *parent = nullptr);
    ~QtNodePropertyWidget();

private:
    void setup_ui();
    void set_selected(QtBaseNode* selected);

    QtBaseNode* _selected_node;

    QWidget* _property_grid;
    
public slots:
    void flow_node_selected(QtBaseNode* node);

private slots:
    void property_changed(const QString& txt);
};

#endif // __QT_NODE_PROPERTY_WIDGET_H__
