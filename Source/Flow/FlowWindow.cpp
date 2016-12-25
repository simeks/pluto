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

FlowWindow::FlowWindow() : _window(nullptr)
{
}
FlowWindow::FlowWindow(QtFlowWindow* window) : _window(window)
{
}
FlowWindow::~FlowWindow()
{
}
void FlowWindow::show()
{
    QMetaObject::invokeMethod(_window, "show");
}
void FlowWindow::close()
{
    QMetaObject::invokeMethod(_window, "close");
}
FlowGraph* FlowWindow::load(const char* )
{
    return nullptr;
}
void FlowWindow::save(const char* )
{
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
FlowWindow::FlowWindow(const FlowWindow& other)
{
    _window = other._window;
}
FlowWindow& FlowWindow::operator=(const FlowWindow& other)
{
    _window = other._window;
    return *this;
}