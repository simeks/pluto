#ifndef __PLUTO_CONSOLE_COMPLETER_H__
#define __PLUTO_CONSOLE_COMPLETER_H__

#include <QString>

class QStringList;
class QTextCursor;
class Completer
{
public:
    Completer();
    ~Completer();

    QString complete(const QString& cmdline, int begin, int end, QStringList& out);

private:
    void complete_python(const QString& cmd, QStringList& out);
    void complete_file(const QString& cmd, QStringList& out);

};


#endif // __PLUTO_CONSOLE_COMPLETER_H__
