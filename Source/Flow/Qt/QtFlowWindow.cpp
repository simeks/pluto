#include <Core/Common.h>

#include "QtFlowWindow.h"
#include "QtFlowGraphScene.h"
#include "QtFlowGraphView.h"

#include <QCoreApplication>
#include <QSettings>
#include <QGridLayout>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>


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
}

QtFlowWindow::~QtFlowWindow()
{
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
        scene->clear_scene();
}

void QtFlowWindow::setup_ui()
{
    QAction* action_exit = new QAction(this);
    action_exit->setObjectName("exit");

    QMenuBar* menu_bar = new QMenuBar(this);
    setMenuBar(menu_bar);

    QMenu* menu_file = new QMenu(menu_bar);
    menu_bar->addAction(menu_file->menuAction());

    menu_file->addSeparator();
    menu_file->addAction(action_exit);

    setWindowTitle("Flow Editor");
    menu_file->setTitle("File");
    action_exit->setText("Exit");

    _graph_view = new QtFlowGraphView(this);
    setCentralWidget(_graph_view);
    _graph_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(this, SIGNAL(node_template_added(FlowNode*)), _graph_view, SLOT(node_template_added(FlowNode*)));
    connect(this, SIGNAL(node_template_removed(FlowNode*)), _graph_view, SLOT(node_template_removed(FlowNode*)));
}
void QtFlowWindow::on_exit_triggered()
{
    close();
}
