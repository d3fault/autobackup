#include "redparenthesisclosecontextrefresherplaintextedit.h"

RedParenthesisCloseContextRefresherPlainTextEdit::RedParenthesisCloseContextRefresherPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), m_OpenParenthesis("("), m_CloseParenthesis(")"), m_CurrentlyHighlighting(false), m_DontTrigger(false)
{
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(handleCursorPositionChanged()));

    QMetaObject::invokeMethod(this, "getFirstPosition", Qt::QueuedConnection);
}
void RedParenthesisCloseContextRefresherPlainTextEdit::setHighlightCurrentContext(bool highlight)
{
    if(!m_CurrentlyHighlighting && !highlight)
    {
        return;
    }

    m_CurrentlyHighlighting = highlight;

    emit d((highlight ? QString("enabling") : QString("disabling")) + QString(" highlighting"));
}
bool RedParenthesisCloseContextRefresherPlainTextEdit::parseParenthesisAndValidate()
{
    m_Document = document()->toPlainText();

    if(m_Document.count(m_OpenParenthesis) != m_Document.count(m_CloseParenthesis))
    {
        return false;
    }

    m_OpenParenthesisLocations.clear();
    m_CloseParenthesisLocations.clear();
    catalogOccurances(&m_OpenParenthesisLocations, m_OpenParenthesis);
    catalogOccurances(&m_CloseParenthesisLocations, m_CloseParenthesis);

    return !tooManyCloseParenthesisInArow();
}
void RedParenthesisCloseContextRefresherPlainTextEdit::catalogOccurances(QList<int> *catalog, QString character)
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
    return false;
}
void RedParenthesisCloseContextRefresherPlainTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_ParenRight)
    {
        //see if it's too many (matching amount), and if so, just SKIP over to the right over one of them if there are any to the right. if there aren't any to the right, do nothing

        //we disable the user the ability to add in a close parenthesis that doesn't belong (but still sanitize input of that fact (when opening a file for example))

        if(parseParenthesisAndValidate())
        {
            e->ignore();

            int currentPosition = textCursor().position();
            QString potentialCloseParenth = m_Document.mid(currentPosition, 1);

            if(potentialCloseParenth == m_CloseParenthesis)
            {
                //todo move over right 1
                m_DontTrigger = true;
                QTextCursor tempCursor = textCursor();
                int origPosition = tempCursor.position();

                tempCursor.setPosition(origPosition+1);
                setTextCursor(tempCursor);
                m_DontTrigger = false;

                setHighlightCurrentContext(true);
            }
            else
            {
                //do nothing. don't move forward, don't put in parenthesis
                emit d("too many close parenthesis. preventing you hahahahahahaha");
            }
        }
    }

    if(e->isAccepted())
    {
        QPlainTextEdit::keyPressEvent(e);
    }
}
void RedParenthesisCloseContextRefresherPlainTextEdit::getFirstPosition()
{
    m_PreviousPosition = textCursor().position();
    /*if(textCursor().movePosition(QTextCursor::Start))
    {
        m_PreviousPosition = textCursor().position();
    }*/
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

    if(parseParenthesisAndValidate())
    {




        int moveDistance = currentPosition - m_PreviousPosition;
        if(moveDistance == 1)
        {
            QString potentialCloseParenth = m_Document.mid(currentPosition-1, 1);
            if(potentialCloseParenth == m_CloseParenthesis)
            {
                setHighlightCurrentContext(true);
            }
            else
            {
                setHighlightCurrentContext(false);
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

            int origPosition = textCursor().position();
            //tempCursor.setKeepPositionOnInsert(true);
            //setTextCursor(tempCursor);

            textCursor().insertText(")");

            //textCursor().setPosition(m_PreviousPosition);
            //textCursor().movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);

            QTextCursor tempCursor = textCursor();
            tempCursor.setPosition(origPosition);
            //textCursor().movePosition(QTextCursor::Left);
            //tempCursor.setKeepPositionOnInsert(false);
            setTextCursor(tempCursor);
            m_DontTrigger = false;
        }
    }

    m_PreviousPosition = currentPosition;
}

void RedParenthesisCloseContextRefresherPlainTextEdit::moveCursorLeft()
{
    /*if(!textCursor().movePosition(QTextCursor::Left))
    {
        emit d("can't move left");
    }*/
}
void RedParenthesisCloseContextRefresherPlainTextEdit::moveCursorRight()
{
    //textCursor().setKeepPositionOnInsert();
    /*if(!textCursor().movePosition(QTextCursor::Right))
    {
        emit d("can't move right");
    }*/
}
