#ifndef __CONSOLE_WIDGET_H__
#define __CONSOLE_WIDGET_H__

#include <Core/Common.h>
#include <QWidget>
#include <QTextEdit>

#include "ConsoleHistory.h"

class CommandInvoker;
class ConsoleWidget : public QTextEdit
{
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = nullptr);
    ~ConsoleWidget();

    /// Appends text before the prompt
    void append_text(const QString& text);

private:
    void keyPressEvent(QKeyEvent *e) OVERRIDE;

    void handle_return();

    void cut();
    void paste();

    void show_prompt();
    void hide_prompt();

    bool cursor_in_prompt(const QTextCursor& cursor) const;

    QString read_prompt() const;

    QString _prompt;
    int _prompt_position;

    CommandInvoker* _command_invoker;

    ConsoleHistory _history;

public slots:
    void kernel_ready();
    void kernel_output(const QString& text);

signals:
    /// Executed when a command is executed
    void invoke_command(const QString& command);
    /// Interrupts the kernel, aborting any ongoing execution
    void interrupt_kernel();
    
};

#endif // __CONSOLE_WIDGET_H__
