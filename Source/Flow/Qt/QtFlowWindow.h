#ifndef __QT_FLOW_WINDOW_H__
#define __QT_FLOW_WINDOW_H__

#include "API.h"
#include <QDialog>
#include <QMainWindow>


class FlowContext;
class FlowGraph;
class FlowNode;
class QtFlowGraphView;
class QtFlowWindow;
class QtNodePropertyWidget;
class QtFlowGraphRunner : public QObject
{
    Q_OBJECT

public:
    QtFlowGraphRunner(QtFlowWindow* window);
    ~QtFlowGraphRunner();

public slots:
    void run(FlowGraph* graph);

private:
    QtFlowWindow* _window;

signals:
    void run_started();
    void run_ended();

    void node_started(FlowNode* node);
    void node_finished(FlowNode* node);
    void node_failed(FlowNode* node);
};

class QtFlowWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtFlowWindow(QWidget *parent = nullptr);
    ~QtFlowWindow();

    FlowGraph* graph();

    void run_graph();
    
public slots:
    void new_graph();
    void load_graph(const QString& file);
    void save_graph(const QString& file);
    void graph_changed();

    void run_graph_started();
    void run_graph_ended();

protected:
    void closeEvent(QCloseEvent* e);

private slots:
    void on_exit_triggered();
    void _set_graph(FlowGraph* graph);
    void _clear_graph();

    void on_new();
    void on_open();
    void on_open_recent();
    void on_clear_recent();
    void on_save();
    void on_save_as();

    void on_run();

    void about();

private:
    void setup_ui();
    void update_recent_menu();
    void set_current_file(const QString& file);
    void set_graph_changed(bool changed);
    void update_title();

    QStringList recent_files() const;
    void add_recent_file(const QString& file);

    static int s_max_num_recent_files;

    QMenu* _recent_menu;
    QtFlowGraphView* _graph_view;
    QtNodePropertyWidget* _node_property_view;
    QString _current_file;
    QString _title_prefix;

    QtFlowGraphRunner* _graph_runner;

    bool _changed;

signals:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);
    void node_template_reloaded(FlowNode* node);
    void set_graph(FlowGraph* graph);
    void clear_graph();
};

#endif // __QT_FLOW_WINDOW_H__
