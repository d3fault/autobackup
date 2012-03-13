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
#include "adagencyapplogic.h"

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
    QLineEdit *m_NewAdOwnerAdCampaignLineEdit;
    QPushButton *m_NewAdOwnerAdCampaignButton;
    QListWidget *m_AllUsersListWidget;
    QListWidget *m_AllAdOwnerAdCampaignsListWidget;
    QMenu *m_ListContextMenu;
    QAction *m_AddFundsRequestAction;

    QPlainTextEdit *m_Debug;

    //objects
    LocalAppBankCache *m_LocalBank;
    QThread *m_LocalBankThread;
    QThread *m_AppLogicThread; //it's on it's own thread because the gui is on the user's browser.. and we post events (http requests) to the app logic on the server...
    AdAgencyAppLogic *m_AppLogic; //not sure if this needs it's own thread or not. in any case, the app logic runs on the Wt server, while the gui runs in the user's browser. that's somewhat like thread separation... so i'm thinking.. yes?

    QString getCurrentlySelectedUsername();
private slots:
    void handleNewUserPressed();
    void handleNewAdCampaignPressed();

    void handleUserAdded(QString newUser);
    void handleCampaignAdded(QString newCampaignName);
    void showListContextMenu(const QPoint &);
    void handleAddFundsRequestActionTriggered();
};

#endif // MAINWEBSITEBANKVIEW_H
