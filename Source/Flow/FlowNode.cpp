#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Python/Sequence.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "FlowProperty.h"

#include "Qt/QtFlowNode.h"

PLUTO_OBJECT_IMPL_DOC(FlowNode, "Node", 
    "FlowNode\n"
    "Base class for nodes\n"
    "\n")
{
    cls.def_init<FlowNode, const char*, const char*, const char*>();
    cls.def("run", &FlowNode::run, 
        "run(ctx)\n"
        "--\n"
        "\n"
        "Args:\n"
        "   ctx : Context\n");
    // TODO: Template args specifies which add_pin to use, ugly so probably needs rethinking
    cls.def<FlowNode, void, FlowPin*>("add_pin", &FlowNode::add_pin, "");
    cls.def("node_id", &FlowNode::node_id, "");
    cls.def("is_pin_linked", &FlowNode::is_pin_linked, "");
	cls.def("set_ui_class", &FlowNode::set_ui_class, "");

    cls.def("property", &FlowNode::property, "");
    cls.def("add_property", &FlowNode::add_property, "");

    cls.def_varargs("invoke_ui_method", &FlowNode::invoke_ui_method, "");
}

FlowNode::FlowNode() :
    _owner_graph(nullptr),
    _function(nullptr),
    _ui_node(nullptr)
{
}
FlowNode::FlowNode(
    const char* node_class,
    const char* title,
    const char* category) :
    _node_class(node_class),
    _title(title),
    _category(category),
    _function(nullptr),
    _owner_graph(nullptr),
    _ui_node(nullptr)
{

}
FlowNode::FlowNode(const FlowNode& other) : 
    Object(other),
    _node_class(other._node_class),
    _title(other._title),
    _category(other._category),
    _owner_graph(other._owner_graph),
    _node_id(other._node_id),
    _function(other._function),
    _ui_class(other._ui_class),
    _ui_pos(other._ui_pos),
    _ui_node(other._ui_node)
{
    for (auto& pin : other._pins)
    {
        FlowPin* p = clone_object(pin);
        p->set_owner(this);
        _pins.push_back(p);
    }
    for (auto& prop : other._properties)
    {
        FlowProperty* p = clone_object(prop);
        p->set_owner(this);
        _properties.push_back(p);
    }
}
FlowNode::~FlowNode()
{
    for (auto p : _pins)
    {
        p->release();
    }
    _pins.clear();
    for (auto p : _properties)
    {
        delete p;
    }
    _properties.clear();
}
void FlowNode::run(FlowContext* ctx)
{
    python::Object method = attribute("run");
    if (is_overridden(method))
    {
        // To avoid recursion we only call "run" if it is actually overriden by Python.
        method(python::make_tuple(ctx));
    }
    else if (_function)
    {
        _function(ctx);
    }
}
const std::vector<FlowPin*>& FlowNode::pins() const
{
    return _pins;
}
FlowPin* FlowNode::pin(const char* name) const
{
    for (auto& p : _pins)
    {
        // Case-insensitive comparison
        if (_stricmp(p->name(), name) == 0)
        {
            return p;
        }
    }
    return nullptr;
}
ArrayFlowPin* FlowNode::pin(const char* name, int index)
{
    std::vector<ArrayFlowPin*> pins = pin_array(name);
    for (auto& p : pins)
    {
        if (p->index() == index)
            return p;
    }

    if (pins.empty())
        return nullptr;

    ArrayFlowPin* last = pins.back();
    while (last->index() < index)
    {
        ArrayFlowPin* pin = clone_object(last);
        pin->set_index(last->index() + 1);
        pin->set_previous(last);
        last->set_next(pin);

        pin->set_owner(this);
        _pins.insert(std::find(_pins.begin(), _pins.end(), last) + 1, pin);

        last = pin;
    }

    return last;
}
std::vector<ArrayFlowPin*> FlowNode::pin_array(const char* name) const
{
    std::vector<ArrayFlowPin*> out;
    for (auto& p : _pins)
    {
        if (p->is_a(ArrayFlowPin::static_class()))
        {
            ArrayFlowPin* pin = object_cast<ArrayFlowPin>(p);

            // Case-insensitive comparison
            if (_stricmp(pin->base_name(), name) == 0)
            {
                // We can assume that all pins come in order
                out.push_back(pin);
            }
        }
    }

    return out;
}
bool FlowNode::is_pin_linked(const char* name) const
{
    FlowPin* p = pin(name);
    if (p)
    {
        return p->links().size() != 0;
    }
    else
    {
        // If no single pin was found, try for pin array

        std::vector<ArrayFlowPin*> pinarr = pin_array(name);
        return pinarr.size() > 1;
    }
}
FlowProperty* FlowNode::property(const char* name) const
{
    for (auto it = _properties.begin(); it != _properties.end(); ++it)
    {
        if (_stricmp((*it)->name(), name) == 0)
            return *it;
    }
    return nullptr;
}
python::Object FlowNode::property_value(const char* name) const
{
    auto prop = property(name);
    if (!prop)
        PYTHON_ERROR(PyExc_KeyError, "no property with name '%s' found.", name);
    return prop->value();
}
const Guid& FlowNode::node_id() const
{
    return _node_id;
}
void FlowNode::set_node_id(const Guid& id)
{
    _node_id = id;
}
FlowGraph* FlowNode::graph() const
{
    return _owner_graph;
}
void FlowNode::set_graph(FlowGraph* graph)
{
    _owner_graph = graph;
}

void FlowNode::add_pin(const char* name, int pin_type)
{
    _pins.push_back(make_object<FlowPin>(name, (FlowPin::Type)pin_type, this));
}
void FlowNode::add_pin(FlowPin* pin)
{
    pin->addref();
    _pins.push_back(pin);
}
void FlowNode::add_property(FlowProperty* prop)
{
    prop->set_owner(this);
    prop->addref();
    _properties.push_back(prop);
}
const std::vector<FlowProperty*>& FlowNode::properties() const
{
    return _properties;
}
const char* FlowNode::node_class() const
{
    return _node_class.c_str();
}
const char* FlowNode::title() const
{
    return _title.c_str();
}
const char* FlowNode::category() const
{
    return _category.c_str();
}
const Vec2i& FlowNode::ui_pos() const
{
    return _ui_pos;
}
void FlowNode::set_ui_pos(const Vec2i& pos)
{
    _ui_pos = pos;
}
const char* FlowNode::ui_class() const
{
    return _ui_class.c_str();
}
void FlowNode::set_ui_class(const char* cls)
{
    _ui_class = cls;
}
void FlowNode::set_ui_node(QtFlowNode* node)
{
    _ui_node = node;
}
namespace
{
    struct ArgVariableBase
    {
        virtual ~ArgVariableBase() {}
    };

    template<typename T>
    struct ArgVariable : public ArgVariableBase
    {
        ArgVariable(const T& obj) : p(obj)
        {
        }
        ~ArgVariable()
        {
        }
        T p;
    };
}
void FlowNode::invoke_ui_method(const python::Tuple& args)
{
    if (!_ui_node)
        PYTHON_ERROR(PyExc_RuntimeError, "FlowNode: No UI object\n");

    if (args.size() < 1)
        PYTHON_ERROR(PyExc_ValueError, "Expected at least one argument");

    const char* meth_name = args.get<const char*>(0);


    QGenericArgument qargs[9];

    /*
    QGenericArgument stores a pointer to the variable that it represents, this of course
    causes a problem with the solution below as all variables are bound to a local scope.

    ArgVariable is an ugly way to solve this, ArgVariable<T> is allocated on the heap
    with the objective to temporary store and then destroy the object after the invokeMethod has
    been called, in this case when qargs_data is destroyed.
    */

    std::vector<std::auto_ptr<ArgVariableBase>> qargs_data;

    assert(args.size() <= 10);

    int i = 0;
    for (; i < args.size() - 1; ++i)
    {
        // TODO: Is there any better way to do this?

        python::Object obj = args.get(i + 1);
        if (PyUnicode_Check(obj.ptr()))
        {
            auto data = new ArgVariable<QString>(python::from_python<QString>(obj));
            qargs_data.push_back(std::auto_ptr<ArgVariableBase>(data));
            qargs[i] = Q_ARG(QString, data->p);
        }
        else if (PyLong_Check(obj.ptr()))
        {
            auto data = new ArgVariable<int>(python::from_python<int>(obj));
            qargs_data.push_back(std::auto_ptr<ArgVariableBase>(data));
            qargs[i] = Q_ARG(int, data->p);
        }
        else if (PyFloat_Check(obj.ptr()))
        {
            auto data = new ArgVariable<float>(python::from_python<float>(obj));
            qargs_data.push_back(std::auto_ptr<ArgVariableBase>(data));
            qargs[i] = Q_ARG(float, data->p);
        }
        else if (numpy::check_type(obj.ptr()))
        {
            auto data = new ArgVariable<Image>(python::from_python<Image>(obj));
            qargs_data.push_back(std::auto_ptr<ArgVariableBase>(data));
            qargs[i] = Q_ARG(Image, data->p);
        }
    }
    if (i <= 9)
        qargs[i] = (QGenericArgument)nullptr;

    QMetaObject::invokeMethod(_ui_node, meth_name, Qt::BlockingQueuedConnection,
        qargs[0], qargs[1], qargs[2],
        qargs[3], qargs[4], qargs[5],
        qargs[6], qargs[7], qargs[8]);

}
void FlowNode::on_pin_linked(FlowPin* pin)
{
    if (pin->is_a(ArrayFlowPin::static_class()))
    {
        add_array_pin(object_cast<ArrayFlowPin>(pin));
    }
}
void FlowNode::on_pin_unlinked(FlowPin* pin)
{
    if (pin->is_a(ArrayFlowPin::static_class()))
    {
        remove_array_pin(object_cast<ArrayFlowPin>(pin));
    }
}
void FlowNode::add_array_pin(ArrayFlowPin* prev)
{
    ArrayFlowPin* first = prev;
    while (first->previous()) first = first->previous();

    ArrayFlowPin* last = first;

    int n = 0; // Number of free pins
    while (last->next())
    {
        if (last->links().size() == 0)
            ++n;

        last = last->next();
    }

    if (n > 0)
        return; // Don't add any new pins to array if we already have free pins
    
    ArrayFlowPin* pin = clone_object(last);
    pin->set_index(last->index() + 1);
    pin->set_previous(last);
    last->set_next(pin);

    pin->set_owner(this);
    _pins.insert(std::find(_pins.begin(), _pins.end(), last)+1, pin);
}
void FlowNode::remove_array_pin(ArrayFlowPin* pin)
{
    // Unlink pin from others
    ArrayFlowPin* next = pin->next();
    ArrayFlowPin* prev = pin->previous();

    if (next) next->set_previous(prev);
    if (prev) prev->set_next(next);

    // Decrement indices of pins after this one
    while (next)
    {
        next->set_index(next->index() - 1);
        next = next->next();
    }
    
    // Remove pin from node
    auto it = std::find(_pins.begin(), _pins.end(), pin);
    assert(it != _pins.end());
    _pins.erase(it);

    pin->release();
}
