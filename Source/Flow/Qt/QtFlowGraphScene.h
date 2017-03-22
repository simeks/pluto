#ifndef __QT_FLOW_DIAGRAM_SCENE_H__
#define __QT_FLOW_DIAGRAM_SCENE_H__

#include <Core/Common.h>
#include <QGraphicsScene>

class FlowGraph;
class FlowNode;
class QtFlowNode;
class QtFlowLink;
class QtFlowPin;
class QtNoteItem;
class QtFlowGraphScene : public QGraphicsScene
{
    Q_OBJECT

public:
    QtFlowGraphScene(QObject *parent = nullptr);
    QtFlowGraphScene(FlowGraph* graph, QObject *parent = nullptr);
    ~QtFlowGraphScene();

    void add_node(QtFlowNode* node);
    void remove_node(QtFlowNode* node);

    //void add_note(QtNoteItem* note);
    //void remove_note(QtNoteItem* note);
    
    /// Called whenever a node template is reloaded.
    /// This reloads all nodes belonging to the node class associated with the template
    void node_template_reloaded(FlowNode* tpl);

    bool try_add_link(QtFlowLink* link);
    void remove_link(QtFlowLink* link);

    /// Finds all links associated to the given pin
    void find_links(QtFlowPin* pin, std::vector<QtFlowLink*>& links) const;

    /// Finds all links associated to the given node
    void find_links(QtFlowNode* node, std::vector<QtFlowLink*>& links) const;

    /// Returns the first link associated to the given pin, useful for in-pins which are limited to only one link
    /// Returns null if no link was found
    QtFlowLink* find_link(QtFlowPin* pin) const;

    void new_graph();

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);
    
    QtFlowNode* node(const Guid& id) const;

    const std::vector<QtNoteItem*>& notes() const;

private:
    void clear_scene();

    /// Removes all links associated with node
    void remove_links(QtFlowNode* node);
    /// Removes all links associated with pin
    void remove_links(QtFlowPin* pin);

    FlowGraph* _flow_graph;

    std::map<Guid, QtFlowNode*> _nodes;
    std::vector<QtFlowLink*> _links;
    std::vector<QtNoteItem*> _notes;

signals:
    void graph_changed();


};


#endif // __QT_FLOW_DIAGRAM_SCENE_H__

