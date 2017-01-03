#ifndef __QT_FLOW_DIAGRAM_SCENE_H__
#define __QT_FLOW_DIAGRAM_SCENE_H__

#include <Core/Common.h>
#include <QGraphicsScene>

class FlowGraph;
class FlowNode;
class QtFlowNode;
class QtFlowLink;
class QtFlowPin;
class QtFlowGraphScene : public QGraphicsScene
{
    Q_OBJECT

public:
    QtFlowGraphScene(QObject *parent = nullptr);
    QtFlowGraphScene(FlowGraph* graph, QObject *parent = nullptr);
    ~QtFlowGraphScene();

    void create_node(FlowNode* node, const QPointF& pos);
    void add_node(QtFlowNode* node);
    void remove_node(QtFlowNode* node);

    bool try_add_link(QtFlowLink* link);
    void remove_link(QtFlowLink* link);

    /// Removes all links associated with node
    void remove_links(QtFlowNode* node);
    /// Removes all links associated with pin
    void remove_links(QtFlowPin* pin);

    /// Finds all links associated to the given pin
    void find_links(QtFlowPin* pin, std::vector<QtFlowLink*>& links) const;

    /// Returns the first link associated to the given pin, useful for in-pins which are limited to only one link
    /// Returns null if no link was found
    QtFlowLink* find_link(QtFlowPin* pin) const;

    void new_graph();

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);
    
private:
    void clear_scene();

    /// Creates the QtFlowNode object corresponding to the specified node class
    QtFlowNode* _create_node(FlowNode* node);

    FlowGraph* _flow_graph;

    std::map<Guid, QtFlowNode*> _nodes;
    std::vector<QtFlowLink*> _links;

};


#endif // __QT_FLOW_DIAGRAM_SCENE_H__

