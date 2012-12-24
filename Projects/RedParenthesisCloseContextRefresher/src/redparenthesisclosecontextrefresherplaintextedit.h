#ifndef REDPARENTHESISCLOSECONTEXTREFRESHERPLAINTEXTEDIT_H
#define REDPARENTHESISCLOSECONTEXTREFRESHERPLAINTEXTEDIT_H

#include <QPlainTextEdit>

class RedParenthesisCloseContextRefresherPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit RedParenthesisCloseContextRefresherPlainTextEdit(QWidget *parent = 0);
private:
    QString m_Document;
    int m_PreviousPosition;
    QString m_OpenParenthesis;
    QString m_CloseParenthesis;

    QList<int> m_OpenParenthesisLocations;
    QList<int> m_CloseParenthesisLocations;

    void setHighlightCurrentContext(bool highlight);
    bool m_CurrentlyHighlighting;
    bool parseParenthesisAndValidate();
    void catalogOccurances(QList<int>* catalog, QString character);
    bool tooManyCloseParenthesisInArow();
    bool m_DontTrigger;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
signals:
    void d(const QString &);
private slots:
    void getFirstPosition();
    void handleCursorPositionChanged();
public slots:
    void moveCursorLeft();
    void moveCursorRight();
};

#endif // REDPARENTHESISCLOSECONTEXTREFRESHERPLAINTEXTEDIT_H
