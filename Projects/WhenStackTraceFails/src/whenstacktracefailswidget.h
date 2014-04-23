#ifndef WHENSTACKTRACEFAILSWIDGET_H
#define WHENSTACKTRACEFAILSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>

#include "spaceactivatedspecialplaintextedit.h"

class WhenStackTraceFailsWidget : public QWidget
{
    Q_OBJECT
public:
    WhenStackTraceFailsWidget(QWidget *parent = 0);
    ~WhenStackTraceFailsWidget();
private:
    QLineEdit *m_FirstHalfLineEdit;
    QLineEdit *m_SecondHalfLineEdit;
    SpaceActivatedSpecialPlainTextEdit *m_DocLolling;
};

#endif // WHENSTACKTRACEFAILSWIDGET_H
