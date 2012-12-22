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

    bool parseParenthesisAndValidate();
    void catelogOccurances(QList<int>* catalog, QString character);
    bool tooManyCloseParenthesisInArow();
    bool m_DontTrigger;
signals:
    void d(const QString &);
private slots:
    void setCursorToBeginning();
    void handleCursorPositionChanged();
};

#endif // REDPARENTHESISCLOSECONTEXTREFRESHERPLAINTEXTEDIT_H
