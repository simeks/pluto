#include <Core/Common.h>
#include <Core/Json/Json.h>
#include <Core/Json/JsonObject.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Pluto/PlutoKernelProxy.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "QtFlowWindow.h"
#include "QtFlowGraphScene.h"
#include "QtFlowGraphView.h"
#include "QtNodePropertyWidget.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QSettings>
#include <QThread>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>


QtFlowGraphRunner::QtFlowGraphRunner(QtFlowWindow* window) : _window(window)
{
    qRegisterMetaType<FlowGraph*>("FlowGraph*");
    this->moveToThread(PlutoCore::instance().kernel_thread());
}
QtFlowGraphRunner::~QtFlowGraphRunner()
{
}
void QtFlowGraphRunner::run(FlowGraph* graph)
{
    QMetaObject::invokeMethod(_window, "setDisabled", Qt::BlockingQueuedConnection, Q_ARG(bool, true));

    PlutoKernelProxy* kernel = PlutoCore::instance().kernel_proxy();
    emit kernel->busy();

    PYTHON_STDOUT("Running graph...\n");

    FlowContext* ctx = object_new<FlowContext>();
    ctx->run(graph);
    ctx->release();

    emit kernel->ready();
    emit finished();

    QMetaObject::invokeMethod(_window, "setDisabled", Q_ARG(bool, false));
}

QtFlowWindow::QtFlowWindow(QWidget *parent) :
    QMainWindow(parent)
{
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
    connect(_graph_runner, SIGNAL(finished()), this, SLOT(update_view()));
}

QtFlowWindow::~QtFlowWindow()
{
    delete _graph_runner;

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
    
    // Use runner if we are on UI thread to avoid UI stalling
    if (thread() == QThread::currentThread())
        QMetaObject::invokeMethod(_graph_runner, "run", Q_ARG(FlowGraph*, graph()));
    else
    {
        QMetaObject::invokeMethod(this, "setDisabled", Qt::BlockingQueuedConnection, Q_ARG(bool, true));

        FlowContext* ctx = object_new<FlowContext>();
        ctx->run(graph());
        ctx->release();

        update_view();
        QMetaObject::invokeMethod(this, "setDisabled", Q_ARG(bool, false));
    }

}
void QtFlowWindow::load_graph(const QString& file)
{
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
    _current_file = file;
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
    _current_file = file;
}
void QtFlowWindow::update_view()
{
    _graph_view->update_nodes();
}
void QtFlowWindow::_set_graph(FlowGraph* graph)
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        scene->set_graph(graph);
}
void QtFlowWindow::_clear_graph()
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        scene->new_graph();
}

void QtFlowWindow::setup_ui()
{
    setWindowTitle("Flow Editor");

    _graph_view = new QtFlowGraphView(this);
    _graph_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

        QAction* action_save = new QAction(tr("Save"), this);
        action_save->setShortcuts(QKeySequence::Save);
        action_save->setStatusTip("Saves the current graph");
        connect(action_save, &QAction::triggered, this, &QtFlowWindow::on_save);

        QAction* action_save_as = new QAction(tr("Save As"), this);
        action_save_as->setShortcuts(QKeySequence::SaveAs);
        action_save_as->setStatusTip("Saves the current graph as");
        connect(action_save_as, &QAction::triggered, this, &QtFlowWindow::on_save_as);

        QAction* action_exit = new QAction(tr("Close"), this);
        action_exit->setShortcuts(QKeySequence::Quit);
        action_exit->setStatusTip("Closes this window");
        connect(action_exit, &QAction::triggered, this, &QtFlowWindow::on_exit_triggered);

        menu_file->addAction(action_new);
        menu_file->addAction(action_open);
        menu_file->addSeparator();
        menu_file->addAction(action_save);
        menu_file->addAction(action_save_as);
        menu_file->addSeparator();
        menu_file->addAction(action_exit);
    }

    {
        QMenu* menu_edit = new QMenu("Edit", menu_bar);
        menu_bar->addAction(menu_edit->menuAction());

        QAction* action_copy = new QAction(tr("Copy"), this);
        action_copy->setShortcuts(QKeySequence::Copy);
        connect(action_copy, &QAction::triggered, _graph_view, &QtFlowGraphView::node_copy);

        QAction* action_paste = new QAction(tr("Paste"), this);
        action_paste->setShortcuts(QKeySequence::Paste);
        connect(action_paste, &QAction::triggered, _graph_view, &QtFlowGraphView::node_paste);

        menu_edit->addAction(action_copy);
        menu_edit->addAction(action_paste);
    }

    {
        QMenu* menu_run = new QMenu("Run", menu_bar);
        menu_bar->addAction(menu_run->menuAction());

        QAction* action_run = new QAction(tr("Run Graph"), this);
        action_run->setShortcut(QKeySequence::fromString("F5"));
        action_run->setStatusTip("Runs the current graph");
        connect(action_run, &QAction::triggered, this, &QtFlowWindow::on_run);

        menu_run->addAction(action_run);
    }

    connect(this, SIGNAL(node_template_added(FlowNode*)), _graph_view, SLOT(node_template_added(FlowNode*)));
    connect(this, SIGNAL(node_template_removed(FlowNode*)), _graph_view, SLOT(node_template_removed(FlowNode*)));

    _node_property_view = new QtNodePropertyWidget(this);
    connect(_graph_view, SIGNAL(flow_node_selected(QtFlowNode*)), _node_property_view, SLOT(flow_node_selected(QtFlowNode*)));

    QWidget* central_widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(central_widget);
    layout->setMargin(0);
    layout->addWidget(_graph_view);
    layout->addWidget(_node_property_view);

    setCentralWidget(central_widget);
}
void QtFlowWindow::on_exit_triggered()
{
    close();
}

void QtFlowWindow::on_new()
{
    QtFlowGraphScene* scene = _graph_view->scene();
    if (scene)
        scene->new_graph();
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
    run_graph();
}
