#include "nothingfancyplaintextedit.h"

NothingFancyPlainTextEdit::NothingFancyPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_TextHasBeenEditted(false)
{
    connect(this, SIGNAL(textChanged()), this, SLOT(setTextHasBeenEdittedToTrue()));
}
bool NothingFancyPlainTextEdit::textHasBeenEditted() const
{
    return m_TextHasBeenEditted;
}
void NothingFancyPlainTextEdit::setTextHasBeenEditted(bool textHasBeenEditted)
{
    m_TextHasBeenEditted = textHasBeenEditted;
}
void NothingFancyPlainTextEdit::setTextHasBeenEdittedToTrue()
{
    m_TextHasBeenEditted = true;
}
