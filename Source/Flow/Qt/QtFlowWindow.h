#ifndef __QT_FLOW_WINDOW_H__
#define __QT_FLOW_WINDOW_H__

#include "API.h"
#include <QDialog>
#include <QMainWindow>

#include <Core/Python/PythonCommon.h>

class FlowContext;
class FlowGraph;
class FlowNode;
class QtFlowGraphView;
class QtFlowLink;
class QtFlowNode;
class QtFlowWindow;
class QtNodePropertyWidget;
class QtNoteItem;
class QUndoStack;
class QtFlowGraphRunner : public QObject
{
    Q_OBJECT

public:
    QtFlowGraphRunner(QtFlowWindow* window);
    ~QtFlowGraphRunner();

    /// Note: Assumes the caller is on the kernel thread
    Dict run(FlowGraph* graph, const Tuple& args, const Dict& kw);

    bool failed() const;

public slots:
    void run(FlowGraph* graph);
    
    /// Resets the runner, meaning the next run will be a full run
    void reset();

private:
    QtFlowWindow* _window;
    FlowContext* _context;

signals:
    void run_started();
    void run_failed(const QString& error);
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

    Dict run_graph(const Tuple& args, const Dict& kw);
  
    /// Do we have a failed run that is waiting?
    bool run_pending();
    
public slots:
    void new_graph();
    void load_graph(const QString& file);
    void save_graph(const QString& file);
    void graph_changed();

    void run_graph_started();
    void run_graph_failed(const QString& error);
    void run_graph_ended();

    /// Reset the run in case previous failed
    void reset_run();

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
    void on_full_run();

    void about();

    void node_create(QtFlowNode* node);
    void link_create(QtFlowLink* link);
    void note_create(QtNoteItem* note);

    void selection_move(const QPointF& old_pos, const QPointF& new_pos);
    void selection_destroy();

private:
    void setup_ui();
    void update_recent_menu();
    void set_current_file(const QString& file);
    void set_graph_changed(bool changed);
    void update_title();

    /// Writes a backup of the current graph
    void perform_backup();
    /// Resets the backup, this should be called before performing a (clean) exit of the application
    void reset_backup();

    /// Checks if latest run resulted in a crash, in that case ask users if it wants to restore any existing backup
    void crash_check();

    /// Asks if the user wants to save
    /// Returns true if the user saved or wants to discard the changes, 
    ///         false if it wants to cancel the current action
    bool ask_save();

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

    QUndoStack* _undo_stack;

signals:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);
    void node_template_reloaded(FlowNode* node);
    void set_graph(FlowGraph* graph);
    void clear_graph();
};

#endif // __QT_FLOW_WINDOW_H__
