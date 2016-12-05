#include <Core/Common.h>

#include "ConsoleWidget.h"

#include <QKeyEvent>
#include <QTextEdit>
#include <QTextDocumentFragment>

#include "CommandInvoker.h"

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QTextEdit(parent),
    _prompt(">>> ")
{
    setFontFamily("Consolas");

    _command_invoker = new CommandInvoker();
    connect(this, SIGNAL(invoke_command(const QString&)), _command_invoker, SLOT(invoke(const QString&)));
    connect(this, SIGNAL(interrupt_kernel()), _command_invoker, SLOT(interrupt()), Qt::DirectConnection);
    connect(_command_invoker, SIGNAL(ready()), this, SLOT(kernel_ready()));
    connect(_command_invoker, SIGNAL(output(const QString&)), this, SLOT(kernel_output(const QString&)));

    setReadOnly(true);

    QMetaObject::invokeMethod(_command_invoker, "start", Qt::AutoConnection);
}

ConsoleWidget::~ConsoleWidget()
{
    QMetaObject::invokeMethod(_command_invoker, "stop", Qt::DirectConnection);
    delete _command_invoker;
    _command_invoker = nullptr;
}
void ConsoleWidget::append_text(const QString& text)
{
    QTextCursor cursor = textCursor();
    if (!isReadOnly())
        cursor.setPosition(_prompt_position-_prompt.length());

    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text);
    _prompt_position = cursor.position() + _prompt.length();
}

void ConsoleWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier)
    {
        if (e->key() == Qt::Key_C && isReadOnly() && !textCursor().hasSelection())
        {
            emit interrupt_kernel();
        }
    }

    if (e->matches(QKeySequence::Cut))
    {
        cut();
        return;
    }
    else if (e->matches(QKeySequence::Paste))
    {
        paste();
        return;
    }

    if (e->modifiers() == 0)
    {
        switch (e->key())
        {
        case Qt::Key_Return:
            handle_return();
            return;
        case Qt::Key_Backspace:
            if (textCursor().position() <= _prompt_position)
            {
                return;
            }
        case Qt::Key_End:
        case Qt::Key_Home:
        case Qt::Key_Left:
        case Qt::Key_Right:
            break;
        case Qt::Key_Up:
            if (cursor_in_prompt(textCursor()))
            {
                if (_history.end())
                    _history.set_prefix(read_prompt());

                QString cmd = _history.find_previous();
                if (!cmd.isEmpty())
                {
                    QTextCursor cursor = textCursor();
                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, _prompt.length());
                    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                    cursor.insertText(cmd);
                    setTextCursor(cursor);
                }
                return;
            }
            break;
        case Qt::Key_Down:
            if (cursor_in_prompt(textCursor()))
            {
                QString cmd = _history.find_next();
                if (!cmd.isEmpty())
                {
                    QTextCursor cursor = textCursor();
                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, _prompt.length());
                    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                    cursor.insertText(cmd);
                    setTextCursor(cursor);
                }
                return;
            }
            break;
        default:
            if (!cursor_in_prompt(textCursor()))
                moveCursor(QTextCursor::End);
            break;
        };
    }
    QTextEdit::keyPressEvent(e);
}
void ConsoleWidget::handle_return()
{
    if (cursor_in_prompt(textCursor()))
    {
        QString cmd = read_prompt();
        if (!cmd.isEmpty())
        {
            hide_prompt();
            _history.push(cmd);
            emit invoke_command(cmd);
            textCursor().insertBlock();
        }
        else
        {
            show_prompt();
        }
    }
    else
    {
        moveCursor(QTextCursor::End);
    }

}
void ConsoleWidget::cut()
{
    if (cursor_in_prompt(textCursor()))
        QTextEdit::cut();
    else
        copy();
}
void ConsoleWidget::paste()
{
    if (cursor_in_prompt(textCursor()))
        QTextEdit::paste();
}
void ConsoleWidget::show_prompt()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    if (cursor.position() > 0)
    {
        if (!toPlainText().endsWith("\n"))
        {
            cursor.insertBlock();
        }
    }

    cursor.insertText(_prompt);
    cursor.movePosition(QTextCursor::EndOfLine);
    _prompt_position = cursor.position();
    setTextCursor(cursor);

    setReadOnly(false);
}
void ConsoleWidget::hide_prompt()
{
    setReadOnly(true);
}
bool ConsoleWidget::cursor_in_prompt(const QTextCursor& cursor) const
{
    return (cursor.selectionStart() >= _prompt_position && cursor.selectionEnd() >= _prompt_position);
}
QString ConsoleWidget::read_prompt() const
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, _prompt.length());
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    return cursor.selection().toPlainText();
}
void ConsoleWidget::kernel_ready()
{
    setReadOnly(false);
    show_prompt();
}
void ConsoleWidget::kernel_output(const QString& text)
{
    append_text(text);
}

