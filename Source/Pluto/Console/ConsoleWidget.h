#ifndef __CONSOLE_WIDGET_H__
#define __CONSOLE_WIDGET_H__

#include <Core/Common.h>
#include <QWidget>
#include <QTextEdit>

#include "Completer.h"
#include "ConsoleHistory.h"

class ConsoleModule;
class PlutoKernelProxy;
class ConsoleWidget : public QTextEdit
{
    Q_OBJECT

public:
    explicit ConsoleWidget(PlutoKernelProxy* kernel, QWidget *parent = nullptr);
    ~ConsoleWidget();

    /// Appends text before the prompt
    void append_html(const QString& text);

private:
    QFont get_font();

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

    PlutoKernelProxy* _kernel_runner;

    ConsoleHistory _history;
    Completer _completer;

    QString _prompt_temp; // Temp storage of prompt value while prompt is hidden

public slots:
    void kernel_ready();
    void kernel_busy();
    void kernel_output(const QString& text, bool html);
    void kernel_error_output(const QString& text);

    void set_style_sheet(const QString& sheet);

    void append_image(const QImage& img);

signals:
    /// Executed when a command is executed
    void invoke_command(const QString& command);
    /// Interrupts the kernel, aborting any ongoing execution
    void interrupt_kernel();
    
};

#endif // __CONSOLE_WIDGET_H__
