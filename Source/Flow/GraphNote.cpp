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
}

const char* GraphNote::text() const
{

}
void GraphNote::set_text(const char* txt)
{

}

const Vec2i& GraphNote::ui_pos() const
{

}
void GraphNote::set_ui_pos(const Vec2i& pos)
{

}


