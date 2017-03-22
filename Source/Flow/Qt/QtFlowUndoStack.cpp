#include <Core/Common.h>

#include "QtFlowGraphScene.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowUndoStack.h"
#include "QtNoteItem.h"

NodeCreateCommand::NodeCreateCommand(
    QtFlowNode* node,
    QtFlowGraphScene* scene,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    _scene(scene),
    _node(node)
{
}
void NodeCreateCommand::undo()
{
    _scene->remove_node(_node);
}
void NodeCreateCommand::redo()
{
    _scene->add_node(_node);
}

LinkCreateCommand::LinkCreateCommand(
    QtFlowLink* link,
    QtFlowGraphScene* scene,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    _scene(scene),
    _link(link)
{
}
void LinkCreateCommand::undo()
{
    _scene->remove_link(_link);
}
void LinkCreateCommand::redo()
{
    _scene->try_add_link(_link);
}

NoteCreateCommand::NoteCreateCommand(
    QtNoteItem* note,
    QtFlowGraphScene* scene,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    _scene(scene),
    _note(note)
{
}
void NoteCreateCommand::undo()
{
    // TODO:
    //_scene->remove_note(_note);
}
void NoteCreateCommand::redo()
{
    // TODO:
    //_scene->add_note(_note);
}

SelectionDestroyCommand::SelectionDestroyCommand(
    QtFlowGraphScene* scene,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    _scene(scene)
{
    _items = _scene->selectedItems();

    std::vector<QtFlowLink*> links;
    for (auto& i : _items)
    {
        if (i->type() == QtFlowNode::Type)
        {
            links.clear();
            _scene->find_links((QtFlowNode*)i, links);
            for (auto& l : links)
            {
                if (!_items.contains(l))
                    _items.push_back(l);
            }
        }
    }
}
void SelectionDestroyCommand::undo()
{
    for (auto& i : _items)
    {
        if (i->type() == QtFlowNode::Type)
        {
            _scene->add_node((QtFlowNode*)i);
        }
        else if (i->type() == QtFlowLink::Type)
        {
            _scene->try_add_link((QtFlowLink*)i);
        }
        /*else if (i->type() == QtNoteItem::Type)
        {
            _scene->add_note((QtNoteItem*)i);
        }*/
    }
}
void SelectionDestroyCommand::redo()
{
    for (auto& i : _items)
    {
        if (i->type() == QtFlowNode::Type)
        {
            _scene->remove_node((QtFlowNode*)i);
        }
        else if (i->type() == QtFlowLink::Type)
        {
            _scene->remove_link((QtFlowLink*)i);
        }
        /*else if (i->type() == QtNoteItem::Type)
        {
            _scene->remove_note((QtNoteItem*)i);
        }*/
    }
}
SelectionMoveCommand::SelectionMoveCommand(
    const QPointF& old_pos,
    const QPointF& new_pos,
    QtFlowGraphScene* scene,
    QUndoCommand *parent) : 
    QUndoCommand(parent),
    _scene(scene),
    _old_pos(old_pos),
    _new_pos(new_pos)
{
    _items = _scene->selectedItems();
}
void SelectionMoveCommand::undo()
{
    QGraphicsItem* n0 = _items[0];
    for (int i = 1; i < _items.size(); ++i)
    {
        QGraphicsItem* ni = _items[i];
        QPointF offset = ni->pos() - n0->pos();

        if (ni->type() == QtFlowNode::Type)
            static_cast<QtFlowNode*>(ni)->move_node(_old_pos + offset);
        else if (ni->type() == QtNoteItem::Type)
            ni->setPos(_old_pos + offset);
    }
    if (n0->type() == QtFlowNode::Type)
        static_cast<QtFlowNode*>(n0)->move_node(_old_pos);
    else if (n0->type() == QtNoteItem::Type)
        n0->setPos(_old_pos);
}
void SelectionMoveCommand::redo()
{
    QGraphicsItem* n0 = _items[0];
    for (int i = 1; i < _items.size(); ++i)
    {
        QGraphicsItem* ni = _items[i];
        QPointF offset = ni->pos() - n0->pos();

        if (ni->type() == QtFlowNode::Type)
            static_cast<QtFlowNode*>(ni)->move_node(_new_pos + offset);
        else if (ni->type() == QtNoteItem::Type)
            ni->setPos(_new_pos + offset);
    }
    if (n0->type() == QtFlowNode::Type)
        static_cast<QtFlowNode*>(n0)->move_node(_new_pos);
    else if (n0->type() == QtNoteItem::Type)
        n0->setPos(_new_pos);
}
