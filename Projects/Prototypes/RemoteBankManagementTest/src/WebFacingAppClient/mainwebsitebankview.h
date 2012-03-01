#ifndef MAINWEBSITEBANKVIEW_H
#define MAINWEBSITEBANKVIEW_H

#include <QtGui/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QPlainTextEdit>

#include "localappbankcache.h"

class mainWebsiteBankView : public QWidget
{
    Q_OBJECT
public:
    mainWebsiteBankView(QWidget *parent = 0);
    ~mainWebsiteBankView();
private:
    //gui
    QVBoxLayout *m_Layout;
    QLineEdit *m_NewUserLineEdit;
    QPushButton *m_NewUserButton;

    QPlainTextEdit *m_Debug;

    //objects
    LocalAppBankCache *m_LocalBank;
    QThread *m_LocalBankThread;
private slots:
    void handleNewUserPressed();
    void handleUserAdded(QString newUser);
};

#endif // MAINWEBSITEBANKVIEW_H
