#ifndef SPACEACTIVATEDSPECIALPLAINTEXTEDIT_H
#define SPACEACTIVATEDSPECIALPLAINTEXTEDIT_H

#include <QPlainTextEdit>

class SpaceActivatedSpecialPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    SpaceActivatedSpecialPlainTextEdit(QWidget * parent = 0);
private:
    int m_CurrentIndex;
    QString m_FirstHalfInsert;
    QString m_SecondHalfInsert;

    void insertLoLAtCurrentLine();
protected:
    virtual void keyPressEvent(QKeyEvent *keyEvent);
public slots:
    void setNewFirstHalf(const QString &newFirstHalf);
    void setNewSecondHalf(const QString &newSecondHalf);
private slots:
    void resetCurrentIndex();
};

#endif // SPACEACTIVATEDSPECIALPLAINTEXTEDIT_H
