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
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QAction>

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
    QListWidget *m_AllUsersListWidget;
    QMenu *m_ListContextMenu;
    QAction *m_AddFundsRequestAction;

    QPlainTextEdit *m_Debug;

    //objects
    LocalAppBankCache *m_LocalBank;
    QThread *m_LocalBankThread;
private slots:
    void handleNewUserPressed();
    void handleUserAdded(QString newUser);
    void showListContextMenu(const QPoint &);
    void handleAddFundsRequestActionTriggered();
};

#endif // MAINWEBSITEBANKVIEW_H
