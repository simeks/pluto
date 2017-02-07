#ifndef __QT_FLOW_UI_H__
#define __QT_FLOW_UI_H__

#include "API.h"
#include <QDialog>

class FlowNode;
class QtFlowNode;
class QtFlowWindow;

class UiNodeFactory
{
public:
    virtual QtFlowNode* create(FlowNode* node) = 0;
};

template<typename T>
class UiNodeFactoryTpl : public UiNodeFactory
{
public:
    QtFlowNode* create(FlowNode* node)
    {
        return new T(node);
    }
};

class FLOW_API QtFlowUI : public QObject
{
    Q_OBJECT

public:
    QtFlowUI();
    ~QtFlowUI();

    QtFlowNode* create_ui_node(FlowNode* node);

    void install_ui_node_factory(const std::string& ui_class, UiNodeFactory* factory);

    template<typename T>
    void install_ui_node_factory(const std::string& ui_class)
    {
        install_ui_node_factory(ui_class, new UiNodeFactoryTpl<T>());
    }

public slots:
    QtFlowWindow* create_window();

signals:
    void node_template_added(FlowNode* n);
    void node_template_removed(FlowNode* n);

    /// Signalled when a node template reloaded
    /// n : Pointer to the new node template
    void node_template_reloaded(FlowNode* n);
    
private:
    std::vector<QtFlowWindow*> _windows;
    std::map<std::string, UiNodeFactory*> _node_factories;

};

#endif // __QT_FLOW_UI_H__
