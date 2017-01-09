#include <Core/Common.h>

#include "FlowGraph.h"
#include "GraphNote.h"

OBJECT_INIT_TYPE_FN(GraphNote)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(GraphNote, "GraphNote", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(GraphNote, Object);

GraphNote::~GraphNote()
{
}

void GraphNote::object_init()
{
    _owner_graph = nullptr;
}
const Guid& GraphNote::id() const
{
    return _id;
}
void GraphNote::set_id(const Guid& id)
{
    _id = id;
}

const char* GraphNote::text() const
{
    return _text.c_str();
}
void GraphNote::set_text(const char* txt)
{
    _text = txt;
}

const Vec2i& GraphNote::ui_pos() const
{
    return _ui_pos;
}
void GraphNote::set_ui_pos(const Vec2i& pos)
{
    _ui_pos = pos;
}
FlowGraph* GraphNote::graph() const
{
    return _owner_graph;
}
void GraphNote::set_graph(FlowGraph* graph)
{
    _owner_graph = graph;
}


