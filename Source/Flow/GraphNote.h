#ifndef __GRAPH_NOTE_H__
#define __GRAPH_NOTE_H__

#include <Core/Object/Object.h>
#include <Core/Image/Vec2.h>

#include "API.h"

class FlowGraph;

class FLOW_API GraphNote : public Object
{
    DECLARE_OBJECT(GraphNote, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(GraphNote);
    ~GraphNote();

    void object_init();
    
    const char* text() const;
    void set_text(const char* txt);

    const Vec2i& ui_pos() const;
    void set_ui_pos(const Vec2i& pos);

protected:
    FlowGraph* _owner_graph;
    std::string _text;
    Vec2i _ui_pos;

};

#endif // __GRAPH_NOTE_H__
