#include <Core/Common.h>
#include <QString>

#include "ConsoleHistory.h"

ConsoleHistory::ConsoleHistory()
    : _index(0)
{
}
ConsoleHistory::~ConsoleHistory()
{
}
void ConsoleHistory::push(const QString& command)
{
    _history.push_back(command);
    _index = int(_history.size());
    _prefix = "";
}
void ConsoleHistory::set_prefix(const QString& prefix)
{
    _index = int(_history.size());
    _prefix = prefix;
}
QString ConsoleHistory::find_next()
{
    int i = _index;
    while (i < _history.size())
    {
        ++i;
        std::cout << i << ", " << _history.size() << ", " << _index << ", " << _prefix.toStdString() << std::endl;
        if (i == _history.size())
        {
            _index = i;
            return _prefix;
        }
        else if (_history[i].startsWith(_prefix))
        {
            _index = i;
            return _history[i];
        }
    }
    return "";
}
QString ConsoleHistory::find_previous()
{
    int i = _index;
    while (i > 0)
    {
        --i;
        if (_history[i].startsWith(_prefix))
        {
            _index = i;
            return _history[i];
        }
    }
    return "";
}
bool ConsoleHistory::end()
{
    return _index == _history.size();
}
