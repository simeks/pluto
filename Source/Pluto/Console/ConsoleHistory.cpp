#include <Core/Common.h>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "ConsoleHistory.h"

ConsoleHistory::ConsoleHistory()
    : _index(0)
{
}
ConsoleHistory::~ConsoleHistory()
{
}
void ConsoleHistory::load(const QString& file)
{
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        QTextStream s(&f);
        while (!s.atEnd())
        {
            QString line = s.readLine();
            _history.push_back(line);
        }
        f.close();
    }
    _index = int(_history.size());
    _prefix = "";
}
void ConsoleHistory::save(const QString& file)
{
    QFile f(file);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream s(&f);
        for (auto& line : _history)
        {
            s << line << '\n';
        }
        f.close();
    }
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
const std::vector<QString>& ConsoleHistory::history() const
{
    return _history;
}
