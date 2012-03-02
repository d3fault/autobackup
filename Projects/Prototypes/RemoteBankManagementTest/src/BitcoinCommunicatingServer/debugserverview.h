#ifndef DEBUGSERVERVIEW_H
#define DEBUGSERVERVIEW_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>

#include "bank.h"

class debugServerView : public QWidget
{
    Q_OBJECT

public:
    debugServerView(QWidget *parent = 0);
    ~debugServerView();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_ListenButton;

    QThread *m_BankThread;
    Bank *m_Bank;
};

#endif // DEBUGSERVERVIEW_H
