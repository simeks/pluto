#ifndef __QT_FLOW_WINDOW_H__
#define __QT_FLOW_WINDOW_H__

#include "API.h"
#include <QDialog>
#include <QMainWindow>

class FlowGraph;
class FlowNode;
class QtFlowGraphView;
class QtFlowWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtFlowWindow(QWidget *parent = nullptr);
    ~QtFlowWindow();

    FlowGraph* graph();
    
    void load_graph(const QString& file);
    void save_graph(const QString& file);

private:
    void setup_ui();

private slots:
    void on_exit_triggered();
    void _set_graph(FlowGraph* graph);
    void _clear_graph();

    void on_new();
    void on_open();
    void on_save();
    void on_save_as();


private:
    QtFlowGraphView* _graph_view;
    QString _current_file;

signals:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);
    void set_graph(FlowGraph* graph);
    void clear_graph();
};

#endif // __QT_FLOW_WINDOW_H__
