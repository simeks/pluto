#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowWindow.h"
#include "Qt/QtFlowWindow.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(FlowWindow, show);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(FlowWindow, close);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowWindow, load, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowWindow, save, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowWindow, graph);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowWindow, set_graph, FlowGraph*);

OBJECT_INIT_TYPE_FN(FlowWindow)
{
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, show, "");
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, close, "");
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, load, "");
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, save, "");
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, graph, "");
    OBJECT_PYTHON_ADD_METHOD(FlowWindow, set_graph, "");
}

IMPLEMENT_OBJECT(FlowWindow, "FlowWindow", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowWindow, Object);


FlowWindow::~FlowWindow()
{
}
void FlowWindow::object_init()
{
    _window = nullptr;
}
void FlowWindow::object_init(QtFlowWindow* window)
{
    _window = window;
}
void FlowWindow::show()
{
    QMetaObject::invokeMethod(_window, "show");
}
void FlowWindow::close()
{
    QMetaObject::invokeMethod(_window, "close");
}
FlowGraph* FlowWindow::load(const char* file)
{
    QMetaObject::invokeMethod(_window, "load_graph", Qt::BlockingQueuedConnection, Q_ARG(QString, file));
    return graph();
}
void FlowWindow::save(const char* file)
{
    QMetaObject::invokeMethod(_window, "save_graph", Qt::BlockingQueuedConnection, Q_ARG(QString, file));
}
FlowGraph* FlowWindow::graph()
{
    FlowGraph* g = _window->graph();
    return object_clone(g);
}
void FlowWindow::set_graph(FlowGraph* graph)
{
    emit _window->set_graph(object_clone(graph));
}
void FlowWindow::clear()
{
    emit _window->clear_graph();
}
