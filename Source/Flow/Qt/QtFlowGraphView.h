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

    void update_nodes();

    void set_scene(QtFlowGraphScene* scene);
    QtFlowGraphScene* scene() const;

public slots:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);
    void node_template_reloaded(FlowNode* tpl);
    void node_copy();
    void node_paste();

    void reset_nodes();
    void node_started(FlowNode* node);
    void node_finished(FlowNode* node);
    void node_failed(FlowNode* node);

    void run_graph_started();
    void run_graph_ended();

protected:
    void mousePressEvent(QMouseEvent* mouse_event);
    void mouseMoveEvent(QMouseEvent* mouse_event);
    void mouseReleaseEvent(QMouseEvent* mouse_event);
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *e);

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
    bool _running;
    QString _running_text;
    QTimer* _running_text_timer;

    QtFlowLink* _temp_link;
    QtFlowPin* _highlight_pin;

    std::vector<QtFlowNode*> _node_clipboard;

    void build_node_menu();
    void draw_background_grid(QPainter* painter, int grid_step);

    /// Tries to find a pin at the specified position, returns null if no pin was found
    QtFlowPin* find_pin(const QPointF& scene_pos);

private slots:
    void show_context_menu(const QPoint&);
    void running_text_anim();

signals:
    void flow_node_selected(QtFlowNode* node);
    void graph_changed();

};


#endif // __QT_FLOW_DIAGRAM_VIEW_H__

