#ifndef __QT_TERMINAL_NODE_H__
#define __QT_TERMINAL_NODE_H__

#include "QtBaseNode.h"

class FlowNode;
class QLabel;
class QtTerminalNode : public QtBaseNode
{
public:
    enum { Type = UserType + 9 };

    QtTerminalNode(FlowNode* node, QGraphicsItem* parent = nullptr);
    ~QtTerminalNode();

    //const std::string& var_name() const;
    //void set_var_name(const std::string& name);

    //std::string value_str() const;
    //void set_value(const std::string& value);

    int type() const;

private:
    QLabel* _name_label;

    void setup();
    //void update_label();
};

#endif // __QT_TERMINAL_NODE_H__
