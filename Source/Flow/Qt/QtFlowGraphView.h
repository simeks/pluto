#ifndef __QT_FLOW_DIAGRAM_VIEW_H__
#define __QT_FLOW_DIAGRAM_VIEW_H__

#include <QGraphicsView>
#include <QTime>

class FlowNode;
class QMenu;
class QtFlowGraphScene;
class QtFlowLink;
class QtFlowNode;
class QtFlowPin;
class QtFlowUI;
class QtNoteItem;
class QtFlowGraphView : public QGraphicsView
{
    Q_OBJECT

public:
    QtFlowGraphView(QWidget *parent = nullptr);
    ~QtFlowGraphView();

    void update_nodes();

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
    void run_graph_failed(const QString& error);
    void run_graph_ended();
    void run_graph_reset();

    void reset_view();

protected:
    void mousePressEvent(QMouseEvent* mouse_event);
    void mouseMoveEvent(QMouseEvent* mouse_event);
    void mouseReleaseEvent(QMouseEvent* mouse_event);
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *e);

    void drawBackground(QPainter * painter, const QRectF & rect);

    QString format_time(const QTime& time);
private:
    enum Mode
    {
        Mode_Nothing,
        Mode_Move,
        Mode_DragPin,
        Mode_Scroll,
        Mode_Select
    };
    enum RunStatus
    {
        RunStatus_Idle,
        RunStatus_Running,
        RunStatus_Failed
    };

    QtFlowUI* _ui;

    QMenu* _node_menu;
    QAction* _note_action;

    QtFlowGraphScene* _scene;

    QPointF _move_start;
    QPoint _last_mouse_pos;

    Mode _mode;
    RunStatus _run_status;
    QString _status_text;
    QTimer* _running_text_timer;
    QTime _run_start_time;

    QtFlowLink* _temp_link;
    QtFlowPin* _highlight_pin;

    static std::vector<QGraphicsItem*> s_node_clipboard;

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

    void node_create(QtFlowNode* node);
    void link_create(QtFlowLink* link);
    void note_create(QtNoteItem* note);

    void selection_move(const QPointF& old_pos, const QPointF& new_pos);
    void selection_destroy();

};


#endif // __QT_FLOW_DIAGRAM_VIEW_H__

