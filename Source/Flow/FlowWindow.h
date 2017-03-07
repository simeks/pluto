#ifndef __FLOW_WINDOW_H__
#define __FLOW_WINDOW_H__

#include "API.h"
#include <Core/Python/Class.h>
#include "Qt/QtFlowWindow.h"

class FlowGraph;
class QtFlowWindow;
class FLOW_API FlowWindow
{
public:
    FlowWindow(QtFlowWindow* window = nullptr);
    ~FlowWindow();

    void show();
    void close();

    FlowGraph* load(const char* file);
    void save(const char* file);

    FlowGraph* graph();
    void set_graph(FlowGraph* graph);
    void clear();

    Dict run(const Tuple& args, const Dict& kw);
    Dict resume();

    static python::Class python_class();

private:
    QtFlowWindow* _window;

};

#endif // __FLOW_WINDOW_H__
