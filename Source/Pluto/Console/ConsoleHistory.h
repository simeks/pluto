#ifndef __PLUTO_CONSOLE_HISTORY_H__
#define __PLUTO_CONSOLE_HISTORY_H__

class ConsoleHistory
{
public:
    ConsoleHistory();
    ~ConsoleHistory();

    void push(const QString& command);

    void set_prefix(const QString& prefix);

    /// Tries to find the next command in the history matching the specified prefix
    QString find_next();

    /// Tries to find the next command in the history matching the specified prefix
    QString find_previous();

    /// Are we at the end of the history log?
    bool end();

private:
    std::vector<QString> _history;
    int _index;

    QString _prefix;
};


#endif // __PLUTO_CONSOLE_HISTORY_H__
