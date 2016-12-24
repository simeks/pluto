#ifndef __QT_FLOW_UI_H__
#define __QT_FLOW_UI_H__

#include "API.h"
#include <QDialog>

class FlowNode;
class QtFlowWindow;
class QtFlowUI : public QObject
{
    Q_OBJECT

public:
    QtFlowUI();
    ~QtFlowUI();
    
public slots:
    QtFlowWindow* create_window();

signals:
    void node_template_added(FlowNode* n);
    void node_template_removed(FlowNode* n);
    
private:
    std::vector<QtFlowWindow*> _windows;

};

#endif // __QT_FLOW_UI_H__
