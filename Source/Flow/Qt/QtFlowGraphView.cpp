#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowLink.h"
#include "QtFlowGraphScene.h"
#include "QtFlowGraphView.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "QtFlowUI.h"
#include "QtNoteItem.h"
#include "Style.h"

#include <QGraphicsItem>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>
#include <QTimer>

std::vector<QGraphicsItem*> QtFlowGraphView::s_node_clipboard;


Q_DECLARE_METATYPE(FlowNode*);

QtFlowGraphView::QtFlowGraphView(QWidget *parent)
    : QGraphicsView(parent),
    _node_menu(nullptr),
    _note_action(nullptr),
    _mode(Mode_Nothing),
    _run_status(RunStatus_Idle),
    _temp_link(nullptr),
    _highlight_pin(nullptr)
{
    _ui = FlowModule::instance().ui();

    build_node_menu();

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_context_menu(const QPoint&)));

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QtFlowGraphScene* graph_scene = new QtFlowGraphScene(this);
    graph_scene->setSceneRect(-2500, -2500, 5000, 5000);
    _scene = graph_scene;
    setScene(graph_scene);

    connect(graph_scene, SIGNAL(graph_changed()), this, SIGNAL(graph_changed()));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheBackground);

    const FlowUIStyle& style = FlowUIStyle::default_style();
    setBackgroundBrush(style.background_color);

    _running_text_timer = new QTimer(this);
    connect(_running_text_timer, SIGNAL(timeout()), this, SLOT(running_text_anim()));

    connect(this, SIGNAL(node_create(QtFlowNode*)), this, SIGNAL(graph_changed()));
    connect(this, SIGNAL(link_create(QtFlowLink*)), this, SIGNAL(graph_changed()));
    connect(this, SIGNAL(note_create(QtNoteItem*)), this, SIGNAL(graph_changed()));
    connect(this, SIGNAL(selection_move(const QPointF&, const QPointF&)), this, SIGNAL(graph_changed()));
    connect(this, SIGNAL(selection_destroy()), this, SIGNAL(graph_changed()));
}

QtFlowGraphView::~QtFlowGraphView()
{
    delete _running_text_timer;
}
void QtFlowGraphView::update_nodes()
{
    for (auto& i : items(viewport()->rect()))
    {
        if (i->type() == QtFlowNode::Type)
            i->update();
    }
}
QtFlowGraphScene* QtFlowGraphView::scene() const
{
    return _scene;
}

void QtFlowGraphView::mousePressEvent(QMouseEvent* mouse_event)
{
    if (_scene && mouse_event->button() & Qt::MouseButton::LeftButton)
    {
        auto scene_items = items(mouse_event->pos());

        QGraphicsItem* item = nullptr;
        for (int i = 0; i < scene_items.size(); ++i)
        {
            if (scene_items[i]->flags() & QGraphicsItem::ItemIsSelectable)
            {
                item = scene_items[i];
                break;
            }
        }

        if (item)
        {
            if (_run_status == RunStatus_Running)
                return;

            if (item->type() == QtFlowNode::Type)
            {
                QtFlowNode* node = (QtFlowNode*)item;
                int pin_id = node->check_pin(node->mapFromScene(mapToScene(mouse_event->pos())));
                if (pin_id != -1)
                {
                    _scene->clearSelection();
                    _scene->clearFocus();

                    if (_temp_link)
                        delete _temp_link;
                    _temp_link = nullptr;

                    QtFlowPin* pin = node->pin(pin_id);
                    if (pin->pin_type() == FlowPin::In && pin->is_linked())
                    {
                        QtFlowLink* link = _scene->find_link(pin);
                        assert(link);

                        _scene->remove_link(link);

                        link->unset_pin(FlowPin::In);
                        _temp_link = link;
                    }
                    else
                    {
                        if (pin->pin_type() == FlowPin::Out)
                        {
                            _temp_link = new QtFlowLink(pin, nullptr);
                        }
                        else
                        {
                            _temp_link = new QtFlowLink(nullptr, pin);
                        }
                    }
                    _temp_link->move_free_end(mapToScene(mouse_event->pos()));
                    _scene->addItem(_temp_link);

                    _mode = Mode_DragPin;
                }
                else
                {
                    if (!_scene->selectedItems().contains(item))
                    {
                        emit flow_node_selected(nullptr);

                        _scene->clearSelection();
                        _scene->clearFocus();

                        node->setSelected(true);

                        emit flow_node_selected((QtFlowNode*)node);

                        _mode = Mode_Move;
                        _move_start = item->pos();
                    }
                    else
                    {
                        _mode = Mode_Move;
                        _move_start = _scene->selectedItems()[0]->pos();
                    }
                }
            }
            else if (item->type() == QtNoteItem::Type)
            {
                if (!_scene->selectedItems().contains(item))
                {
                    emit flow_node_selected(nullptr);

                    _scene->clearSelection();
                    _scene->clearFocus();

                    item->setSelected(true);

                    _mode = Mode_Move;
                    _move_start = item->pos();
                }
                else
                {
                    _mode = Mode_Move;
                    _move_start = _scene->selectedItems()[0]->pos();
                }
            }
        }
        else if (mouse_event->modifiers() & Qt::AltModifier || mouse_event->buttons() & Qt::MiddleButton)
        {
            _mode = Mode_Scroll;
            setDragMode(DragMode::ScrollHandDrag);
            QGraphicsView::mousePressEvent(mouse_event);

            _scene->clearSelection();
            _scene->clearFocus();
        }


        if (_mode == Mode_Nothing)
        {
            _mode = Mode_Select;
            setDragMode(DragMode::RubberBandDrag);
            QGraphicsView::mousePressEvent(mouse_event);
        }
    }
    else
    {
        setDragMode(DragMode::NoDrag);
        QGraphicsView::mousePressEvent(mouse_event);
    }
    _scene->update();
    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseMoveEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_Move:
        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)
            {
                QPointF delta = mapToScene(mouse_event->pos()) - mapToScene(_last_mouse_pos);
                ((QtFlowNode*)item)->move_node(item->pos() + delta);
            }
            else if (item->type() == QtNoteItem::Type)
            {
                QPointF delta = mapToScene(mouse_event->pos()) - mapToScene(_last_mouse_pos);
                item->setPos(item->pos() + delta);
            }
        }
        break;
    case Mode_DragPin:
    {
        if (_highlight_pin)
        {
            _highlight_pin->set_highlight(false);
            _highlight_pin = nullptr;
        }

        // Is target_pin compatible?

        QtFlowPin* target_pin = find_pin(mapToScene(mouse_event->pos()));
        if (target_pin)
        {
            QtFlowPin* source_pin = (target_pin->pin_type() == FlowPin::In) ? _temp_link->start() : _temp_link->end();
            if (source_pin && target_pin->owner() != source_pin->owner())
            {
                _highlight_pin = target_pin;
                _highlight_pin->set_highlight(true);
            }
        }
        _temp_link->move_free_end(mapToScene(mouse_event->pos()));
        _temp_link->update();
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    case Mode_Scroll:
    {
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    case Mode_Select:
    {
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    default:
        QGraphicsView::mouseMoveEvent(mouse_event);
    };
    update();

    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseReleaseEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_Move:
    {
        if ((_move_start - _scene->selectedItems()[0]->pos()).manhattanLength() > 0)
        {
            emit selection_move(_move_start, _scene->selectedItems()[0]->pos());
        }
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    case Mode_DragPin:
    {
        if (!_temp_link)
            break;

        if (_highlight_pin)
        {
            _temp_link->set_pin(_highlight_pin);
            if (_scene->try_add_link(_temp_link))
            {
                emit link_create(_temp_link);
                _temp_link = nullptr;
            }
            _highlight_pin->set_highlight(false);
            _highlight_pin = nullptr;
        }

        if (_temp_link)
        {
            delete _temp_link;
            _temp_link = nullptr;
        }

        break;
    }
    case Mode_Scroll:
    {
        setDragMode(DragMode::NoDrag);
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    case Mode_Select:
    {
        QGraphicsView::mouseMoveEvent(mouse_event);

        emit flow_node_selected(nullptr);
        if (_scene->selectedItems().size() == 1)
        {
            QGraphicsItem* item = _scene->selectedItems()[0];
            if (item->type() == QtFlowNode::Type)
                emit flow_node_selected((QtFlowNode*)item);
        }

    }
    default:
        QGraphicsView::mouseMoveEvent(mouse_event);
    };

    _scene->update();
    _last_mouse_pos = mouse_event->pos();

    _mode = Mode_Nothing;
}
void QtFlowGraphView::keyPressEvent(QKeyEvent *e)
{
    if (_run_status == RunStatus_Running)
    {
        e->ignore();
        return;
    }

    if (e->key() == Qt::Key_Delete)
    {
        if (_scene->selectedItems().size())
            emit selection_destroy();

        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)
            {
                QtFlowNode* node = (QtFlowNode*)item;
                _scene->remove_node(node);
            }
            else if (item->type() == QtFlowLink::Type)
            {
                QtFlowLink* link = (QtFlowLink*)item;
                _scene->remove_link(link);
            }
        }

        _scene->clearSelection();

        _mode = Mode_Nothing;
    }

    _scene->update();
    QGraphicsView::keyPressEvent(e);
}
void QtFlowGraphView::wheelEvent(QWheelEvent *e)
{
    qreal zoom = e->delta()*0.0005;

    QTransform t = transform();
    t.scale(1 + zoom, 1 + zoom);
    setTransform(t);

    _scene->update();
}

void QtFlowGraphView::drawBackground(QPainter * painter, const QRectF & r)
{
    QGraphicsView::drawBackground(painter, r);

    QBrush brush = backgroundBrush();

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setPen(QPen(style.grid_color_0, 1.0));
    draw_background_grid(painter, 15);

    painter->setPen(QPen(style.grid_color_1, 1.0));
    draw_background_grid(painter, 150);
}
void QtFlowGraphView::draw_background_grid(QPainter* painter, int grid_step)
{
    QPointF tl = mapToScene(rect().topLeft());
    QPointF br = mapToScene(rect().bottomRight());

    int left = std::floor(tl.x() / grid_step - 0.5);
    int right = std::floor(br.x() / grid_step + 1.0);
    int top = std::floor(tl.y() / grid_step - 0.5);
    int bottom = std::floor(br.y() / grid_step + 1.0);

    // vertical 
    for (int x = left; x <= right; ++x)
    {
        QLineF line(x * grid_step, top * grid_step, x * grid_step, bottom * grid_step);
        painter->drawLine(line);
    }

    // horizontal 
    for (int y = top; y <= bottom; ++y)
    {
        QLineF line(left * grid_step, y * grid_step, right * grid_step, y * grid_step);
        painter->drawLine(line);
    }
}
void QtFlowGraphView::build_node_menu()
{
    if (!_node_menu)
        _node_menu = new QMenu(this);
    else
        _node_menu->clear();

    if (!_note_action)
        _note_action = new QAction("Note");
    _node_menu->addAction(_note_action);
    _node_menu->addSeparator();

    std::vector<FlowNode*> nodes = FlowModule::instance().node_templates();
    if (!nodes.empty())
    {
        std::map<std::string, QMenu*> sub_menus;
        sub_menus[""] = _node_menu;

        std::sort(nodes.begin(), nodes.end(), [](FlowNode* x, FlowNode* y)
        {
            int c = strcmp(x->category(), y->category());
            if (c < 0)
                return true;
            else if (c > 0)
                return false;

            return strcmp(x->title(), y->title()) < 0;
        });

        // Build menus
        for (auto& node : nodes)
        {
            if (sub_menus.find(node->category()) == sub_menus.end())
            {
                std::vector<std::string> elems;

                std::stringstream ss(node->category());
                std::string item;
                while (std::getline(ss, item, '/'))
                {
                    elems.push_back(item);
                }


                QMenu* target_menu = _node_menu;
                std::string menu_str = "";
                for (int i = 0; i < int(elems.size()); ++i)
                {
                    if (menu_str != "")
                        menu_str += "/";
                    menu_str += elems[i];
                    auto it = sub_menus.find(menu_str);
                    if (it == sub_menus.end())
                    {
                        sub_menus[menu_str] = new QMenu(QString::fromStdString(elems[i]), target_menu);
                        target_menu->addMenu(sub_menus[menu_str]);
                    }
                    target_menu = sub_menus[menu_str];
                }
            }
        }

        // Insert nodes
        for (auto& node : nodes)
        {
            QAction* node_action = new QAction(QString::fromStdString(node->title()), _node_menu);
            node_action->setData(QVariant::fromValue<FlowNode*>(node));

            QMenu* target_menu = sub_menus[node->category()];
            target_menu->addAction(node_action);

        }
    }
}
QtFlowPin* QtFlowGraphView::find_pin(const QPointF& scene_pos)
{
    for (auto& item : items(mapFromScene(scene_pos)))
    {
        if (item->type() == QtFlowNode::Type)
        {
            QtFlowNode* node = (QtFlowNode*)item;
            int pin_id = node->check_pin(item->mapFromScene(scene_pos));
            if (pin_id != -1)
                return node->pin(pin_id);
        }
    }
    return nullptr;
}
void QtFlowGraphView::show_context_menu(const QPoint& pt)
{
    if (!_scene)
        return;

    auto scene_items = items(pt);
    if (scene_items.size())
        return;

    if (_node_menu)
    {
        QPoint global_pt = mapToGlobal(pt);

        QAction* action = _node_menu->exec(global_pt);
        if (action)
        {
            if (action == _note_action)
            {
                QtNoteItem* note = new QtNoteItem();
                note->setPos(mapToScene(pt));
                emit note_create(note);
            }
            else
            {
                FlowNode* template_node = action->data().value<FlowNode*>();
                if (template_node)
                {
                    FlowNode* node = object_cast<FlowNode>(object_clone(template_node));
                    node->set_node_id(guid::create_guid());

                    QtFlowNode* n = _ui->create_ui_node(node);
                    n->move_node(mapToScene(pt));
                    _scene->add_node(n);

                    emit node_create(n);
                }
            }
        }
    }
}
void QtFlowGraphView::node_template_added(FlowNode*)
{
    // Rebuild node menu
    build_node_menu();
}
void QtFlowGraphView::node_template_removed(FlowNode*)
{
    // Rebuild node menu
    build_node_menu();
}
void QtFlowGraphView::node_template_reloaded(FlowNode* tpl)
{
    build_node_menu();
    _scene->node_template_reloaded(tpl);
}
void QtFlowGraphView::node_copy()
{
    s_node_clipboard.clear();
    for (auto& i : _scene->selectedItems())
    {
        s_node_clipboard.push_back(i);
    }
}
void QtFlowGraphView::node_paste()
{
    if (!s_node_clipboard.empty() && underMouse())
    {
        std::map<Guid, QtFlowNode*> old_to_new;

        QPointF mouse_pos = mapToScene(mapFromGlobal(QCursor::pos()));
        QPointF origin = s_node_clipboard[0]->scenePos();
        for (auto i : s_node_clipboard)
        {
            if (i->type() == QtFlowNode::Type)
            {
                QtFlowNode* node = (QtFlowNode*)i;

                QPointF offset = node->scenePos() - origin;
                FlowNode* copy = object_clone(node->node());
                copy->set_node_id(guid::create_guid());

                QtFlowNode* n = _ui->create_ui_node(copy);
                n->move_node(mouse_pos + offset);
                _scene->add_node(n);

                emit node_create(n);
                copy->release();

                old_to_new[node->node_id()] = n;
            }
        }

        // Connect nodes
        for (auto i : s_node_clipboard)
        {
            if (i->type() == QtFlowLink::Type)
            {
                QtFlowLink* link = (QtFlowLink*)i;

                QtFlowPin* old_start = link->start();
                QtFlowPin* old_end = link->end();

                if (old_to_new.find(old_start->owner()->node_id()) == old_to_new.end() ||
                    old_to_new.find(old_end->owner()->node_id()) == old_to_new.end())
                    continue;

                QtFlowNode* new_start_node = old_to_new[old_start->owner()->node_id()];
                QtFlowNode* new_end_node = old_to_new[old_end->owner()->node_id()];

                QtFlowLink* new_link = new QtFlowLink(new_start_node->pin(old_start->name()), new_end_node->pin(old_end->name()));
                _scene->try_add_link(new_link);
            }
        }

    }
}
void QtFlowGraphView::reset_nodes()
{
    for (auto& i : items())
    {
        if (i->type() == QtFlowNode::Type)
            ((QtFlowNode*)i)->reset_run_status();
    }
}
void QtFlowGraphView::node_started(FlowNode* node)
{
    QtFlowNode* n = _scene->node(node->node_id());
    if (n)
        n->node_started();
}
void QtFlowGraphView::node_finished(FlowNode* node)
{
    QtFlowNode* n = _scene->node(node->node_id());
    if (n)
        n->node_finished();
}
void QtFlowGraphView::node_failed(FlowNode* node)
{
    QtFlowNode* n = _scene->node(node->node_id());
    if (n)
        n->node_failed();
    _run_status = RunStatus_Failed;
}
void QtFlowGraphView::run_graph_started()
{
    if (_run_status == RunStatus_Idle)
        reset_nodes();

    _run_status = RunStatus_Running;
    _run_start_time.restart();

    emit flow_node_selected(nullptr);
    _scene->clearSelection();
    _scene->clearFocus();
    _mode = Mode_Nothing;

    _status_text = "Running";
    _running_text_timer->start(100);
}
void QtFlowGraphView::run_graph_failed(const QString& error)
{
    _run_status = RunStatus_Failed;
    update_nodes();

    _status_text = error;
    _running_text_timer->stop();
}
void QtFlowGraphView::run_graph_ended()
{
    _run_status = RunStatus_Idle;
    update_nodes();

    _status_text = QString("Successful, Elapsed: %1").arg(format_time(_run_start_time));
    _running_text_timer->stop();
}
void QtFlowGraphView::run_graph_reset()
{
    _run_status = RunStatus_Idle;
    reset_nodes();

    _status_text = "";
    _running_text_timer->stop();
}
void QtFlowGraphView::reset_view()
{
    resetTransform();
}
void QtFlowGraphView::paintEvent(QPaintEvent *e)
{
    // Draw scene
    QGraphicsView::paintEvent(e);

    // Draw overlay
    QPainter painter(viewport());

    painter.setRenderHints(QPainter::Antialiasing);

    QFont fnt;
    fnt.setBold(true);

    if (_run_status == RunStatus_Failed)
    {
        painter.setFont(fnt);
        painter.setPen(Qt::black);
        painter.drawText(QPoint(7, 17), _status_text);
        painter.setPen(Qt::red);
        painter.drawText(QPoint(5, 15), _status_text);
    }
    else
    {
        painter.setFont(fnt);
        painter.setPen(Qt::black);
        painter.drawText(QPoint(7, 17), _status_text);
        painter.setPen(Qt::white);
        painter.drawText(QPoint(5, 15), _status_text);
    }
}
void QtFlowGraphView::running_text_anim()
{
    //static int dot_count = 1;

    _status_text = QString("Running, Elapsed: %1").arg(format_time(_run_start_time));
    /*for (int i = 0; i < dot_count; ++i)
    {
        _status_text.append(".");
    }
    ++dot_count;
    if (dot_count > 3)
        dot_count = 0;
*/
    resetCachedContent();
    update();
}
QString QtFlowGraphView::format_time(const QTime& time)
{
    int s = time.elapsed() / 1000;
    if (s < 60)
    {
        return QString("%1 s").arg(s);
    }
    else if (s < 3600)
    {
        int m = s / 60;
        s = s % 60;
        if (s == 0)
            return QString("%1 m").arg(m);
        else
            return QString("%1 m %2 s").arg(m).arg(s);
    }
    else
    {
        int m = s / 60;
        int h = m / 60;
        m = m % 60;
        s = s % 60;
        if (s == 0 && m == 0)
            return QString("%1 h").arg(h);
        else if (s == 0)
            return QString("%1 h %2 m").arg(h).arg(m);
        else
            return QString("%1 h %2 m %3 s").arg(h).arg(m).arg(s);
    }
}
