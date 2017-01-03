#ifndef __QT_FLOW_DIAGRAM_VIEW_H__
#define __QT_FLOW_DIAGRAM_VIEW_H__

#include <QGraphicsView>

class FlowNode;
class QMenu;
class QtFlowGraphScene;
class QtFlowLink;
class QtFlowNode;
class QtFlowPin;
class QtFlowGraphView : public QGraphicsView
{
    Q_OBJECT

public:
    QtFlowGraphView(QWidget *parent = nullptr);
    ~QtFlowGraphView();

    void set_scene(QtFlowGraphScene* scene);
    QtFlowGraphScene* scene() const;

public slots:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);

protected:
    void mousePressEvent(QMouseEvent* mouse_event);
    void mouseMoveEvent(QMouseEvent* mouse_event);
    void mouseReleaseEvent(QMouseEvent* mouse_event);
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);

    void drawBackground(QPainter * painter, const QRectF & rect);
private:
    enum Mode
    {
        Mode_Nothing,
        Mode_Move,
        Mode_DragPin,
        Mode_Scroll
    };

    QMenu* _node_menu;

    QtFlowGraphScene* _scene;

    QPoint _last_mouse_pos;

    Mode _mode;

    QtFlowLink* _temp_link;
    QtFlowPin* _highlight_pin;

    void build_node_menu();
    void draw_background_grid(QPainter* painter, int grid_step);

    /// Tries to find a pin at the specified position, returns null if no pin was found
    QtFlowPin* find_pin(const QPointF& scene_pos);

private slots:
    void show_context_menu(const QPoint&);

signals:
    void flow_node_selected(QtFlowNode* node);

};


#endif // __QT_FLOW_DIAGRAM_VIEW_H__

