#include "redparenthesisclosecontextrefresherplaintextedit.h"

RedParenthesisCloseContextRefresherPlainTextEdit::RedParenthesisCloseContextRefresherPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), m_OpenParenthesis("("), m_CloseParenthesis(")"), m_DontTrigger(false)
{
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(handleCursorPositionChanged()));

    QMetaObject::invokeMethod(this, "setCursorToBeginning", Qt::QueuedConnection);
}
bool RedParenthesisCloseContextRefresherPlainTextEdit::parseParenthesisAndValidate()
{
    if(m_Document.count(m_OpenParenthesis) != m_Document.count(m_CloseParenthesis))
    {
        return false;
    }

    m_OpenParenthesisLocations.clear();
    m_CloseParenthesisLocations.clear();
    catelogOccurances(&m_OpenParenthesisLocations, m_OpenParenthesis);
    catelogOccurances(&m_CloseParenthesisLocations, m_CloseParenthesis);

    return !tooManyCloseParenthesisInArow();
}
void RedParenthesisCloseContextRefresherPlainTextEdit::catelogOccurances(QList<int> *catalog, QString character)
{
    int currentOccurance = 0;
    while(true)
    {
        currentOccurance = m_Document.indexOf(character, currentOccurance+1);
        if(currentOccurance != -1)
        {
            catalog->append(currentOccurance);
        }
        else
        {
            break;
        }
    }
}
bool RedParenthesisCloseContextRefresherPlainTextEdit::tooManyCloseParenthesisInArow()
{
    int parenthesisCount = m_OpenParenthesisLocations.size(); //they are even so it could have come from close
    for(int i = 0; i < parenthesisCount; ++i)
    {
        int openLocation = m_OpenParenthesisLocations.at(i);
        int closeLocation = m_CloseParenthesisLocations.at(i);

        if(openLocation > closeLocation)
        {
            return true;
        }
    }
    emit d("valid parenthesis");
    return false;
}
void RedParenthesisCloseContextRefresherPlainTextEdit::setCursorToBeginning()
{
    if(textCursor().movePosition(QTextCursor::Start))
    {
        m_PreviousPosition = textCursor().position();
    }
}
void RedParenthesisCloseContextRefresherPlainTextEdit::handleCursorPositionChanged()
{
    if(m_DontTrigger)
    {
        return;
    }

    int currentPosition = textCursor().position();
    if(currentPosition == m_PreviousPosition)
        return;


    m_Document = document()->toPlainText();
    if(parseParenthesisAndValidate())
    {




        int moveDistance = currentPosition - m_PreviousPosition;
        if(moveDistance == 1)
        {
            QString potentialCloseParenth = m_Document.mid(currentPosition-1, 1);
            emit d(potentialCloseParenth);
            if(potentialCloseParenth == m_CloseParenthesis)
            {
                //emit d("close parenthesis detected");
            }
            else
            {
                //emit d("stopping highlighting");
            }
        }
    }
    else
    {
        QString potentialOpenParenth = m_Document.mid(currentPosition-1, 1);
        emit d(potentialOpenParenth);
        if(potentialOpenParenth == m_OpenParenthesis)
        {
            m_DontTrigger = true;
            //m_PreviousPosition = currentPosition;
            textCursor().insertText(")");
            //textCursor().setPosition(m_PreviousPosition);
            textCursor().movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
            m_DontTrigger = false;
        }
    }

    m_PreviousPosition = currentPosition;
}
