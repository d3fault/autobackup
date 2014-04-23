#include "spaceactivatedspecialplaintextedit.h"

#include <QDebug>

SpaceActivatedSpecialPlainTextEdit::SpaceActivatedSpecialPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_CurrentIndex(0)
{
    setWordWrapMode(QTextOption::NoWrap); //fucking word wrap in plaintextedit makes this fucked (goes to where wrap, instead of where end of line REALLY (arguably) is)
}
void SpaceActivatedSpecialPlainTextEdit::insertLoLAtCurrentLine()
{
    textCursor().clearSelection();

    QTextCursor blah(textCursor());

    QString firstHalfWithNumber = m_FirstHalfInsert;
    firstHalfWithNumber.replace("%i", QString::number(m_CurrentIndex++));
    blah.insertText(firstHalfWithNumber);
    blah.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    setTextCursor(blah);
    blah.insertText(m_SecondHalfInsert);
    blah.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    blah.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    setTextCursor(blah);
}
void SpaceActivatedSpecialPlainTextEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->key() == Qt::Key_Space)
    {
        insertLoLAtCurrentLine();
        return;
    }
    QPlainTextEdit::keyPressEvent(keyEvent);
}
void SpaceActivatedSpecialPlainTextEdit::setNewFirstHalf(const QString &newFirstHalf)
{
    m_FirstHalfInsert = newFirstHalf;
}
void SpaceActivatedSpecialPlainTextEdit::setNewSecondHalf(const QString &newSecondHalf)
{
    m_SecondHalfInsert = newSecondHalf;
}
void SpaceActivatedSpecialPlainTextEdit::resetCurrentIndex()
{
    m_CurrentIndex = 0;
}
