#ifndef __PLUTO_CONSOLE_COMPLETER_H__
#define __PLUTO_CONSOLE_COMPLETER_H__

class QString;
class QStringList;
class Completer
{
public:
    Completer();
    ~Completer();

    void complete(const QString& cmd, QStringList& out);

};


#endif // __PLUTO_CONSOLE_COMPLETER_H__
