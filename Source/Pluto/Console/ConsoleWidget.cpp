#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>

#include "ConsoleWidget.h"
#include "ConsoleModule.h"

#include <QKeyEvent>
#include <QStyle>
#include <QTextEdit>
#include <QTextDocumentFragment>

#include <Core/Pluto/PlutoKernelProxy.h>

//static const char* console_stylesheet =
//"QPlainTextEdit, QTextEdit { background-color: white; color: black; selection-background-color: blue; }"
//".error { color: red; white-space: pre-wrap; }"
//".output { color: black; white-space: pre-wrap; }";

static const char* console_stylesheet =
"QPlainTextEdit, QTextEdit { background-color: #1E1E1E; color: #DADADA; selection-background-color: #264F78; }"
".error { color: #FC3E36; white-space: pre-wrap; }"
".output { color: #DADADA; white-space: pre-wrap; }";

static const char* s_history_file = "console_history.txt";

ConsoleWidget::ConsoleWidget(PlutoKernelProxy* kernel, QWidget *parent) :
    QTextEdit(parent),
    _prompt(">>> ")
{
    _kernel_runner = kernel;
    connect(this, SIGNAL(invoke_command(const QString&)), _kernel_runner, SLOT(invoke(const QString&)));
    connect(this, SIGNAL(interrupt_kernel()), _kernel_runner, SLOT(interrupt()), Qt::DirectConnection);
    connect(_kernel_runner, SIGNAL(ready()), this, SLOT(kernel_ready()));
    connect(_kernel_runner, SIGNAL(busy()), this, SLOT(kernel_busy()));
    connect(_kernel_runner, SIGNAL(output(const QString&, bool)), this, SLOT(kernel_output(const QString&, bool)));
    connect(_kernel_runner, SIGNAL(error_output(const QString&)), this, SLOT(kernel_error_output(const QString&)));

    setReadOnly(true);

    QFont font = get_font();
    setFont(font);
    document()->setDefaultFont(font);

    setStyleSheet(console_stylesheet);
    document()->setDefaultStyleSheet(console_stylesheet);

    ConsoleModule::instance().set_widget(this);

    QString history_file = QString("%1/%2").arg(PlutoCore::instance().user_dir(), s_history_file);
    _history.load(history_file);
}

ConsoleWidget::~ConsoleWidget()
{
    QString history_file = QString("%1/%2").arg(PlutoCore::instance().user_dir(), s_history_file);
    _history.save(history_file);
}
void ConsoleWidget::append_text(const QString& text)
{
    QTextCursor cursor = textCursor();
    if (!isReadOnly())
        cursor.setPosition(_prompt_position-_prompt.length());

    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml(text.toHtmlEscaped());
    _prompt_position = cursor.position() + _prompt.length();
}
void ConsoleWidget::append_html(const QString& text)
{
    QTextCursor cursor = textCursor();
    if (!isReadOnly())
        cursor.setPosition(_prompt_position - _prompt.length());
    else
        cursor.movePosition(QTextCursor::End);
    cursor.insertHtml(text);
    _prompt_position = cursor.position() + _prompt.length();
}
QFont ConsoleWidget::get_font()
{
    QFont font = QFont("Consolas", 10);

    QFontInfo font_info = QFontInfo(font);
    if (font_info.family() != "Consolas")
    {
        // TODO:
        std::cout << "Error: Could not find font 'Consolas'." << std::endl;
    }
    return font;
}

void ConsoleWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        if (e->key() == Qt::Key_C && isReadOnly() && !textCursor().hasSelection())
        {
            emit interrupt_kernel();
            return;
        }
        else if (e->matches(QKeySequence::Cut))
        {
            cut();
            return;
        }
        else if (e->matches(QKeySequence::Paste))
        {
            paste();
            return;
        }
    }
    else
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
            break;
        case Qt::Key_End:
        case Qt::Key_Home:
            break;
        case Qt::Key_Left:
            if (!cursor_in_prompt(textCursor()))
            {
                moveCursor(QTextCursor::End);
                return;
            }
            if (textCursor().position() <= _prompt_position)
            {
                return;
            }
            break;
        case Qt::Key_Right:
            if (!cursor_in_prompt(textCursor()))
            {
                moveCursor(QTextCursor::End);
                return;
            }
            break;
        case Qt::Key_Up:
        {
            if (!cursor_in_prompt(textCursor()))
                moveCursor(QTextCursor::End);

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
        {
            if (!cursor_in_prompt(textCursor()))
                moveCursor(QTextCursor::End);

            QString cmd = _history.find_next();
            //if (!cmd.isEmpty())
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
    moveCursor(QTextCursor::End);

    if (cursor_in_prompt(textCursor()))
    {
        QString cmd = read_prompt();
        if (!cmd.isEmpty())
        {
            setReadOnly(true);
            _history.push(cmd);
            emit invoke_command(cmd);

            textCursor().insertBlock();
        }
        else
        {
            show_prompt();
        }
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
        cursor.insertBlock();
    }

    cursor.insertHtml(_prompt.toHtmlEscaped());
    cursor.movePosition(QTextCursor::EndOfLine);
    _prompt_position = cursor.position();

    if (!_prompt_temp.isEmpty())
    {
        cursor.insertText(_prompt_temp);
        _prompt_temp = "";
    }

    setTextCursor(cursor);

    setReadOnly(false);
}
void ConsoleWidget::hide_prompt()
{
    _prompt_temp = read_prompt();

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
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
void ConsoleWidget::kernel_busy()
{
    hide_prompt();
}
void ConsoleWidget::kernel_output(const QString& text, bool html)
{
    if (html)
    {
        QString formatted = text;
        formatted.replace("\n", "<br/>");
        append_html(formatted);
    }
    else
    {
        QString formatted = QString("<span class='output'>%1</span>").arg(text.toHtmlEscaped());
        formatted.replace("\n", "<br/>");
        append_html(formatted);
    }
}
void ConsoleWidget::kernel_error_output(const QString& text)
{
    QString formatted = QString("<span class='error'>%1</span>").arg(text);
    formatted.replace("\n", "<br/>");
    append_html(formatted);
}
void ConsoleWidget::set_style_sheet(const QString& sheet)
{
    setStyleSheet(sheet);
    document()->setDefaultStyleSheet(sheet);
}

