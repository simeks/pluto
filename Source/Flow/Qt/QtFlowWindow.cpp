#include <Core/Common.h>
#include <Core/Json/Json.h>
#include <Core/Json/JsonObject.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Pluto/PlutoKernelProxy.h>
#include <Core/Python/PythonCommon.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "QtFlowWindow.h"
#include "QtFlowGraphScene.h"
#include "QtFlowGraphView.h"
#include "QtFlowUndoStack.h"
#include "QtNodePropertyWidget.h"
#include "QtNoteItem.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QThread>
#include <QUndoStack>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

namespace
{
    class QtFlowGraphRunnerCallback : public FlowContext::Callback
    {
    public:
        QtFlowGraphRunnerCallback(QtFlowGraphRunner* r) : _runner(r) {}

        virtual void node_started(FlowNode* node)
        {
            emit _runner->node_started(node);
        }
        virtual void node_finished(FlowNode* node)
        {
            emit _runner->node_finished(node);
        }
        virtual void node_failed(FlowNode* node)
        {
            emit _runner->node_failed(node);
        }

    private:
        QtFlowGraphRunner* _runner;
    };
}

QtFlowGraphRunner::QtFlowGraphRunner(QtFlowWindow* window) : _window(window), _context(nullptr)
{
    qRegisterMetaType<FlowGraph*>("FlowGraph*");
    this->moveToThread(PlutoCore::instance().kernel_thread());
}
QtFlowGraphRunner::~QtFlowGraphRunner()
{
    if (_context)
        _context->release();
}
Dict QtFlowGraphRunner::run(FlowGraph* , const Tuple& , const Dict& kw)
{
    emit run_started();

    PYTHON_STDOUT("Running graph...\n");

    if (!_context)
    {
        //_context = python::make_object<FlowContext>(graph);

        if (kw.valid())
        {
            for (auto& it : _context->inputs())
            {
                _context->set_input(it.first.c_str(), kw.get(it.first.c_str()));
            }
        }
    }

    QtFlowGraphRunnerCallback cb(this);
    if (!_context->run(&cb))
    {
        emit run_failed(_context->error());
        return Dict();
    }

    Dict ret;
    for (auto& it : _context->outputs())
    {
        python::Object obj = _context->output(it.first.c_str());
        if (!obj.is_none())
        {
            ret.set(it.first.c_str(), obj);
        }
        else
        {
            ret.set(it.first.c_str(), python::None());
        }
    }

    // If the run was sucessful we destroy the context, 
    // If not we save the state for future re-runs
    _context->release();
    _context = nullptr;

    emit run_ended();
    return ret;

}
bool QtFlowGraphRunner::failed() const
{
    return _context != nullptr;
}
void QtFlowGraphRunner::run(FlowGraph* )
{
    emit run_started();

    PlutoKernelProxy* kernel = PlutoCore::instance().kernel_proxy();
    emit kernel->busy();

    PYTHON_STDOUT("Running graph...\n");

    //if (!_context)
    //    _context = python::make_object<FlowContext>(graph);
    
    QtFlowGraphRunnerCallback cb(this);
    if (_context->run(&cb))
    {
        // If the run was sucessful we destroy the context, 
        // If not we save the state for future re-runs
        _context->release();
        _context = nullptr;
    }
    
    emit kernel->ready();

    if (_context && _context->failed())
        emit run_failed(_context->error());
    else
        emit run_ended();
}
void QtFlowGraphRunner::reset()
{
    if (_context)
    {
        _context->release();
        _context = nullptr;
    }
}

int QtFlowWindow::s_max_num_recent_files = 5;

QtFlowWindow::QtFlowWindow(QWidget *parent) :
    QMainWindow(parent),
    _recent_menu(nullptr),
    _changed(false)
{
    setWindowIcon(QIcon(":/res/icon.ico"));
    _undo_stack = new QUndoStack(this);

    setMinimumSize(800, 600);

    QSettings settings;

    settings.beginGroup("flowwindow");

    restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
    restoreState(settings.value("savestate", saveState()).toByteArray());
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size", size()).toSize());
    if (settings.value("maximized", isMaximized()).toBool())
        showMaximized();

    settings.endGroup();

    setup_ui();

    connect(this, SIGNAL(set_graph(FlowGraph*)), SLOT(_set_graph(FlowGraph*)));
    connect(this, SIGNAL(clear_graph()), SLOT(_clear_graph()));

    _graph_runner = new QtFlowGraphRunner(this);
    connect(_graph_runner, SIGNAL(run_started()), this, SLOT(run_graph_started()), Qt::BlockingQueuedConnection);
    connect(_graph_runner, SIGNAL(run_failed(const QString&)), this, SLOT(run_graph_failed(const QString&)), Qt::BlockingQueuedConnection);
    connect(_graph_runner, SIGNAL(run_ended()), this, SLOT(run_graph_ended()), Qt::BlockingQueuedConnection);
    connect(_graph_runner, SIGNAL(node_started(FlowNode*)), _graph_view, SLOT(node_started(FlowNode*)));
    connect(_graph_runner, SIGNAL(node_finished(FlowNode*)), _graph_view, SLOT(node_finished(FlowNode*)));
    connect(_graph_runner, SIGNAL(node_failed(FlowNode*)), _graph_view, SLOT(node_failed(FlowNode*)));

    crash_check();
}

QtFlowWindow::~QtFlowWindow()
{
    delete _undo_stack;
    delete _graph_runner;
}
FlowGraph* QtFlowWindow::graph()
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        return scene->graph();
    return nullptr;
}
void QtFlowWindow::run_graph()
{
    if (!graph())
        return;
    
    QMetaObject::invokeMethod(_graph_runner, "run", Q_ARG(FlowGraph*, graph()));
}
Dict QtFlowWindow::run_graph(const Tuple& args, const Dict& kw)
{
    if (!graph())
        return Dict();
    return _graph_runner->run(graph(), args, kw);
}
void QtFlowWindow::reset_run()
{
    _graph_runner->reset();
    _graph_view->reset_nodes();
}
bool QtFlowWindow::run_pending()
{
    return _graph_runner->failed();
}

void QtFlowWindow::new_graph()
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        scene->new_graph();

    set_current_file("");
    set_graph_changed(false);

    _graph_view->reset_view();
}
void QtFlowWindow::load_graph(const QString& file)
{
    if (!ask_save())
    {
        return;
    }

    JsonObject obj;
    JsonReader reader;
    if (!reader.read_file(file.toStdString(), obj))
    {
        QMessageBox::warning(this, tr("Flow"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(file), reader.error_message().c_str()));
        return;
    }

    FlowGraph* graph = flow_graph::load(obj);
    if (!graph)
    {
        QMessageBox::warning(this, tr("Flow"), tr("Failed to parse flow file."));
        return;
    }

    set_graph(graph);

    set_current_file(file);
    add_recent_file(file);
    set_graph_changed(false);

    _graph_view->reset_view();
}
void QtFlowWindow::save_graph(const QString& file)
{
    JsonObject obj;
    flow_graph::save(graph(), obj);

    JsonWriter writer;
    if (!writer.write_file(obj, file.toStdString(), true))
    {
        QMessageBox::warning(this, tr("Flow"), tr("Cannot write file %1.").arg(QDir::toNativeSeparators(file)));
        return;
    }
    set_current_file(file);
    set_graph_changed(false);
    add_recent_file(file);
}
void QtFlowWindow::graph_changed()
{
    set_graph_changed(true);
}
void QtFlowWindow::run_graph_started()
{
    menuBar()->setDisabled(true);
    _node_property_view->setDisabled(true);
    _graph_view->run_graph_started();
    _title_prefix = "[Running] ";
    update_title();
}
void QtFlowWindow::run_graph_failed(const QString& error)
{
    menuBar()->setDisabled(false);
    _node_property_view->setDisabled(false);
    _graph_view->run_graph_failed(error);
    _title_prefix = "[Failed] ";
    update_title();
}
void QtFlowWindow::run_graph_ended()
{
    menuBar()->setDisabled(false);
    _node_property_view->setDisabled(false);
    _graph_view->run_graph_ended();
    _title_prefix = "";
    update_title();
}
void QtFlowWindow::set_graph_changed(bool changed)
{
    _changed = changed;
    update_title();
}
void QtFlowWindow::update_title()
{
    if (_changed)
    {
        if (!_current_file.isEmpty())
        {
            QFileInfo f(_current_file);
            setWindowTitle(_title_prefix + f.fileName() + "* - Flow Editor");
        }
        else
        {
            setWindowTitle(_title_prefix + "New* - Flow Editor");
        }
    }
    else
    {
        if (!_current_file.isEmpty())
        {
            QFileInfo f(_current_file);
            setWindowTitle(_title_prefix + f.fileName() + " - Flow Editor");
        }
        else
        {
            setWindowTitle(_title_prefix + "New - Flow Editor");
        }
    }
}
void QtFlowWindow::perform_backup()
{
    // The backup actions should be silent and avoid modifiying the recent files list

    if (!_changed)
        return;

    QSettings settings;
    settings.beginGroup("flowwindow");
    settings.setValue("last_file", _current_file);
    settings.endGroup();

    JsonObject obj;
    flow_graph::save(graph(), obj);

    JsonWriter writer;
    std::string backup_file = PlutoCore::instance().user_dir();
    backup_file += "/";
    backup_file += "backup.flow";
    if (!writer.write_file(obj, backup_file, true))
    {
        QMessageBox::warning(this, tr("Flow"), tr("Cannot write file %1.").arg(QDir::toNativeSeparators(backup_file.c_str())));
        return;
    }
    set_graph_changed(false);
}
void QtFlowWindow::reset_backup()
{
    std::string backup_file = PlutoCore::instance().user_dir();
    backup_file += "/";
    backup_file += "backup.flow";

    QFile f(QString::fromStdString(backup_file));
    if (f.exists())
    {
        f.remove();
    }
}
void QtFlowWindow::crash_check()
{
    // The backup actions should be silent and avoid modifiying the recent files list

    std::string backup_file = PlutoCore::instance().user_dir();
    backup_file += "/";
    backup_file += "backup.flow";
    QFile f(QString::fromStdString(backup_file));
    if (f.exists())
    {
        const QMessageBox::StandardButton ret
            = QMessageBox::question(this, tr("Flow Editor"),
                tr("Crash backup detected.\n"
                    "Do you want restore the backup?"),
                QMessageBox::Yes | QMessageBox::No);

        if (ret != QMessageBox::Yes)
        {
            reset_backup();
            return;
        }

        JsonObject obj;
        JsonReader reader;
        if (!reader.read_file(backup_file, obj))
        {
            QMessageBox::warning(this, tr("Flow"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(backup_file.c_str()), reader.error_message().c_str()));
            reset_backup();
            return;
        }

        FlowGraph* graph = flow_graph::load(obj);
        if (!graph)
        {
            QMessageBox::warning(this, tr("Flow"), tr("Failed to parse flow file."));
            reset_backup();
            return;
        }

        set_graph(graph);

        QSettings settings;
        settings.beginGroup("flowwindow");
        set_current_file(settings.value("last_file").toString());
        settings.endGroup();

        set_graph_changed(true);

        _graph_view->reset_view();
    }
    reset_backup();
}
void QtFlowWindow::_set_graph(FlowGraph* graph)
{
    QtFlowGraphScene* scene = _graph_view->scene();
    scene->set_graph(graph);
    _graph_view->reset_nodes();
}
void QtFlowWindow::_clear_graph()
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        scene->new_graph();
}
QStringList QtFlowWindow::recent_files() const
{
    QSettings settings;
    settings.beginGroup("flowwindow");

    QStringList files = settings.value("recent").toStringList();
    settings.endGroup();
    return files;
}
void QtFlowWindow::add_recent_file(const QString& file)
{
    QSettings settings;
    settings.beginGroup("flowwindow");

    QStringList files = settings.value("recent").toStringList();
    if (!files.contains(file))
    {
        files.push_front(file);
        if (files.size() > s_max_num_recent_files)
            files.pop_back();
    }
    else
    {
        files.removeOne(file);
        files.push_front(file);
    }

    settings.setValue("recent", files);
    settings.endGroup();

    update_recent_menu();
}
void QtFlowWindow::setup_ui()
{
    update_title();

    _graph_view = new QtFlowGraphView(this);
    _graph_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(_graph_view, SIGNAL(graph_changed()), this, SLOT(graph_changed()));
    connect(_graph_view, SIGNAL(node_create(QtFlowNode*)), this, SLOT(node_create(QtFlowNode*)));
    connect(_graph_view, SIGNAL(link_create(QtFlowLink*)), this, SLOT(link_create(QtFlowLink*)));
    connect(_graph_view, SIGNAL(note_create(QtNoteItem*)), this, SLOT(note_create(QtNoteItem*)));
    connect(_graph_view, SIGNAL(selection_move(const QPointF&, const QPointF&)), this, SLOT(selection_move(const QPointF&, const QPointF&)));
    connect(_graph_view, SIGNAL(selection_destroy()), this, SLOT(selection_destroy()));

    QMenuBar* menu_bar = new QMenuBar(this);
    setMenuBar(menu_bar);

    {
        QMenu* menu_file = new QMenu("File", menu_bar);
        menu_bar->addAction(menu_file->menuAction());

        QAction* action_new = new QAction(tr("New"), this);
        action_new->setShortcuts(QKeySequence::New);
        action_new->setStatusTip("Creates a new graph");
        connect(action_new, &QAction::triggered, this, &QtFlowWindow::on_new);

        QAction* action_open = new QAction(tr("Open"), this);
        action_open->setShortcuts(QKeySequence::Open);
        action_open->setStatusTip("Opens an existing graph");
        connect(action_open, &QAction::triggered, this, &QtFlowWindow::on_open);

        _recent_menu = new QMenu("Open Recent", menu_file);

        QAction* action_save = new QAction(tr("Save"), this);
        action_save->setShortcuts(QKeySequence::Save);
        action_save->setStatusTip("Saves the current graph");
        connect(action_save, &QAction::triggered, this, &QtFlowWindow::on_save);

        QAction* action_save_as = new QAction(tr("Save As"), this);
        action_save_as->setShortcuts(QKeySequence::SaveAs);
        action_save_as->setStatusTip("Saves the current graph as");
        connect(action_save_as, &QAction::triggered, this, &QtFlowWindow::on_save_as);

        QAction* action_exit = new QAction(tr("Exit"), this);
        action_exit->setShortcuts(QKeySequence::Quit);
        action_exit->setStatusTip("Closes this window");
        connect(action_exit, &QAction::triggered, this, &QtFlowWindow::on_exit_triggered);

        menu_file->addAction(action_new);
        menu_file->addAction(action_open);
        menu_file->addAction(_recent_menu->menuAction());
        menu_file->addSeparator();
        menu_file->addAction(action_save);
        menu_file->addAction(action_save_as);
        menu_file->addSeparator();
        menu_file->addAction(action_exit);
    }

    {
        QMenu* menu_edit = new QMenu("Edit", menu_bar);
        menu_bar->addAction(menu_edit->menuAction());

        QAction* action_undo = _undo_stack->createUndoAction(this, tr("Undo"));
        action_undo->setShortcuts(QKeySequence::Undo);

        QAction* action_redo = _undo_stack->createRedoAction(this, tr("Redo"));
        action_redo->setShortcuts(QKeySequence::Redo);

        QAction* action_copy = new QAction(tr("Copy"), this);
        action_copy->setShortcuts(QKeySequence::Copy);
        connect(action_copy, &QAction::triggered, _graph_view, &QtFlowGraphView::node_copy);

        QAction* action_paste = new QAction(tr("Paste"), this);
        action_paste->setShortcuts(QKeySequence::Paste);
        connect(action_paste, &QAction::triggered, _graph_view, &QtFlowGraphView::node_paste);

        menu_edit->addAction(action_undo);
        menu_edit->addAction(action_redo);
        menu_edit->addSeparator();
        menu_edit->addAction(action_copy);
        menu_edit->addAction(action_paste);
    }

    {
        QMenu* menu_view = new QMenu("View", menu_bar);
        menu_bar->addAction(menu_view->menuAction());

        QAction* action_reset = new QAction(tr("Reset"), this);
        connect(action_reset, &QAction::triggered, _graph_view, &QtFlowGraphView::reset_view);

        menu_view->addAction(action_reset);
    }

    {
        QMenu* menu_run = new QMenu("Run", menu_bar);
        menu_bar->addAction(menu_run->menuAction());

        QAction* action_run = new QAction(tr("Run Graph"), this);
        action_run->setShortcut(QKeySequence::fromString("F5"));
        action_run->setStatusTip("Runs the current graph");
        connect(action_run, &QAction::triggered, this, &QtFlowWindow::on_run);

        QAction* action_full_run = new QAction(tr("Full Run Graph"), this);
        action_full_run->setShortcut(QKeySequence::fromString("Ctrl+F5"));
        action_full_run->setStatusTip("Runs the current graph from start");
        connect(action_full_run, &QAction::triggered, this, &QtFlowWindow::on_full_run);

        QAction* action_reset = new QAction(tr("Reset"), this);
        action_reset->setStatusTip("Resets the graph");
        connect(action_reset, &QAction::triggered, this, &QtFlowWindow::reset_run);

        menu_run->addAction(action_run);
        menu_run->addAction(action_full_run);
        menu_run->addAction(action_reset);
    }

    {
        QMenu* menu_help = new QMenu("Help", menu_bar);
        menu_bar->addAction(menu_help->menuAction());

        QAction* action_about = new QAction(tr("About Flow"), this);
        menu_help->addAction(action_about);
        connect(action_about, &QAction::triggered, this, &QtFlowWindow::about);
    }

    connect(this, SIGNAL(node_template_added(FlowNode*)), _graph_view, SLOT(node_template_added(FlowNode*)));
    connect(this, SIGNAL(node_template_removed(FlowNode*)), _graph_view, SLOT(node_template_removed(FlowNode*)));
    connect(this, SIGNAL(node_template_reloaded(FlowNode*)), _graph_view, SLOT(node_template_reloaded(FlowNode*)));

    _node_property_view = new QtNodePropertyWidget(this);
    connect(_graph_view, SIGNAL(flow_node_selected(QtFlowNode*)), _node_property_view, SLOT(flow_node_selected(QtFlowNode*)));


    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(_graph_view);
    splitter->addWidget(_node_property_view);
    _node_property_view->resize(600, 0); // TODO: Sizing is weird

    setCentralWidget(splitter);

    update_recent_menu();
}
void QtFlowWindow::update_recent_menu()
{
    _recent_menu->clear();

    QStringList files = recent_files();
    for (auto& f : files)
    {
        QAction* action = new QAction(f, this);
        action->setData(f);
        connect(action, &QAction::triggered, this, &QtFlowWindow::on_open_recent);

        _recent_menu->addAction(action);
    }
    if (_recent_menu->isEmpty())
        _recent_menu->setDisabled(true);
    else
        _recent_menu->setDisabled(false);

    _recent_menu->addSeparator();
    QAction* clear_action = new QAction("Clear Items", this);
    connect(clear_action, &QAction::triggered, this, &QtFlowWindow::on_clear_recent);
    _recent_menu->addAction(clear_action);
}
void QtFlowWindow::closeEvent(QCloseEvent* e)
{
    if (!ask_save())
    {
        e->ignore();
        return;
    }

    reset_backup();

    QSettings settings;
    settings.beginGroup("flowwindow");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("savestate", saveState());
    settings.setValue("maximized", isMaximized());
    if (!isMaximized())
    {
        settings.setValue("pos", pos());
        settings.setValue("size", size());
    }

    settings.endGroup();
}
bool QtFlowWindow::ask_save()
{
    if (_changed)
    {
        const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Flow Editor"),
                tr("The graph has been modified.\n"
                    "Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        switch (ret)
        {
        case QMessageBox::Save:
            on_save();
            return true;
        case QMessageBox::Cancel:
            return false;
        }
    }
    return true;
}
void QtFlowWindow::on_exit_triggered()
{
    close();
}

void QtFlowWindow::on_new()
{
    new_graph();
}
void QtFlowWindow::on_open()
{
    if (_graph_view)
    {
        QString file_name = QFileDialog::getOpenFileName(this, "Open File", "", "Flow (*.flow)");
        if (file_name != "")
        {
            load_graph(file_name);
        }
    }
}
void QtFlowWindow::on_open_recent()
{
    if (_graph_view)
    {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
        {
            load_graph(action->data().toString());
        }
    }
}
void QtFlowWindow::on_clear_recent()
{
    QSettings settings;

    settings.beginGroup("flowwindow");
    settings.setValue("recent", QStringList());
    settings.endGroup();

    update_recent_menu();
}
void QtFlowWindow::on_save()
{
    if (!_current_file.isEmpty())
        save_graph(_current_file);
    else
        on_save_as();
}
void QtFlowWindow::on_save_as()
{
    if (_graph_view)
    {
        QString file_name = QFileDialog::getSaveFileName(this, "Save File", "", "Flow (*.flow)");
        if (file_name != "")
        {
            save_graph(file_name);
        }
    }
}
void QtFlowWindow::on_run()
{
    perform_backup();
    run_graph();
}
void QtFlowWindow::on_full_run()
{
    perform_backup();
    _graph_view->run_graph_reset();
    _graph_runner->reset();
    run_graph();
}
void QtFlowWindow::about()
{
    QMessageBox::about(this, tr("About Flow"),
        tr("Developed by Simon Ekström (simon.ekstrom@surgsci.uu.se)"));
}
void QtFlowWindow::set_current_file(const QString& file)
{
    _current_file = file;
    update_title();
}

void QtFlowWindow::node_create(QtFlowNode* node)
{
    _undo_stack->push(new NodeCreateCommand(node, _graph_view->scene()));

    _graph_runner->reset();
    _graph_view->run_graph_reset();
}
void QtFlowWindow::link_create(QtFlowLink* link)
{
    _undo_stack->push(new LinkCreateCommand(link, _graph_view->scene()));

    _graph_runner->reset();
    _graph_view->run_graph_reset();
}
void QtFlowWindow::note_create(QtNoteItem* note)
{
    _undo_stack->push(new NoteCreateCommand(note, _graph_view->scene()));
}
void QtFlowWindow::selection_move(const QPointF& old_pos, const QPointF& new_pos)
{
    _undo_stack->push(new SelectionMoveCommand(old_pos, new_pos, _graph_view->scene()));
}
void QtFlowWindow::selection_destroy()
{
    _undo_stack->push(new SelectionDestroyCommand(_graph_view->scene()));

    _graph_runner->reset();
    _graph_view->run_graph_reset();
}
