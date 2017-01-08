#ifndef __QT_FLOW_UNDO_STACK_H__
#define __QT_FLOW_UNDO_STACK_H__

#include <QUndoCommand>

class QtFlowGraphScene;
class QtFlowNode;

class NodeCreateCommand : public QUndoCommand
{
public:
    NodeCreateCommand(QtFlowNode* node, QtFlowGraphScene* scene, QUndoCommand *parent = 0);

    void undo() OVERRIDE;
    void redo() OVERRIDE;

private:
    QtFlowGraphScene* _scene;
    QtFlowNode* _node;
};

class LinkCreateCommand : public QUndoCommand
{
public:
    LinkCreateCommand(QtFlowLink* link, QtFlowGraphScene* scene, QUndoCommand *parent = 0);

    void undo() OVERRIDE;
    void redo() OVERRIDE;

private:
    QtFlowGraphScene* _scene;
    QtFlowLink* _link;
};

class SelectionDestroyCommand : public QUndoCommand
{
public:
    SelectionDestroyCommand(QtFlowGraphScene* scene, QUndoCommand *parent = 0);

    void undo() OVERRIDE;
    void redo() OVERRIDE;
private:
    QtFlowGraphScene* _scene;
    QList<QGraphicsItem*> _items;
};

class SelectionMoveCommand : public QUndoCommand
{
public:
    SelectionMoveCommand(
        const QPointF& old_pos, 
        const QPointF& new_pos, 
        QtFlowGraphScene* scene, 
        QUndoCommand *parent = 0);

    void undo() OVERRIDE;
    void redo() OVERRIDE;

private:
    QtFlowGraphScene* _scene;
    QList<QGraphicsItem*> _items;
    QPointF _old_pos;
    QPointF _new_pos;
};


#endif // __QT_FLOW_UNDO_STACK_H__
