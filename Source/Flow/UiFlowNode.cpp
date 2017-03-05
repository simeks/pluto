#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Python/Sequence.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowPin.h"
#include "FlowProperty.h"
#include "UiFlowNode.h"

#include "Qt/QtFlowNode.h"

#include <QObject>

PYTHON_FUNCTION_WRAPPER_CLASS_TUPLE(UiFlowNode, invoke_ui_method);

OBJECT_INIT_TYPE_FN(UiFlowNode)
{
    OBJECT_PYTHON_ADD_METHOD(UiFlowNode, invoke_ui_method, "");
}

IMPLEMENT_OBJECT_DOC(UiFlowNode, "UiFlowNode", FLOW_API,
    "UiFlowNode\n"
    "Specialized base class for nodes providing utilities for interaction with the UI\n"
    "\n"
    "Attributes:\n"
    "   node_class (str): Unique class name for this type of node.\n"
    "   title (str): Title visible in the UI\n"
    "   category (str): Category of the form 'Category/Subcategory'\n");

IMPLEMENT_OBJECT_CONSTRUCTOR(UiFlowNode, FlowNode);

UiFlowNode::UiFlowNode(const UiFlowNode& other) : FlowNode(other)
{
    _ui_node = nullptr;
}
void UiFlowNode::object_init()
{
    _owner_graph = nullptr;
    _function = nullptr;
    _ui_node = nullptr;
}
void UiFlowNode::object_init(const FlowNodeDef& def)
{
    _owner_graph = nullptr;
    _function = def.fn;
    _ui_node = nullptr;

    if (def.pins)
    {
        FlowPinDef* pin = def.pins;
        while (pin->name)
        {
            // TODO: Pin doc
            add_pin(pin->name, pin->type);
            ++pin;
        }
    }
}
void UiFlowNode::object_python_init(const Tuple&, const Dict&)
{
    _owner_graph = nullptr;
    _function = nullptr;
    _ui_node = nullptr;

    Dict d = get_class()->dict();
    if (d.has_key("pins"))
    {
        Sequence pins = Sequence(d.get("pins"));
        for (size_t i = 0; i < pins.size(); ++i)
        {
            FlowPin* pin = python::from_python<FlowPin*>(pins.get(i).ptr());
            add_pin(object_clone(pin));
        }
    }
    if (d.has_key("properties"))
    {
        Sequence props = Sequence(d.get("properties"));
        for (size_t i = 0; i < props.size(); ++i)
        {
            FlowProperty* prop = python::from_python<FlowProperty*>(props.get(i).ptr());
            add_property(object_clone(prop));
        }
    }
}
UiFlowNode::~UiFlowNode()
{
    for (auto p : _pins)
    {
        p->release();
    }
    _pins.clear();
    for (auto p : _properties)
    {
        p->release();
    }
    _properties.clear();
}
void UiFlowNode::set_ui_node(QtFlowNode* node)
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
void UiFlowNode::invoke_ui_method(const Tuple& args)
{
    if (!_ui_node)
        PYTHON_STDERR("UiFlowNode: No UI object\n");

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
