#include "mainwebsitebankview.h"

mainWebsiteBankView::mainWebsiteBankView(QWidget *parent)
    : QWidget(parent)
{
    m_Debug = new QPlainTextEdit();

    m_LocalBankThread = new QThread();
    m_LocalBank = new LocalAppBankCache();
    m_LocalBank->moveToThread(m_LocalBankThread);
    m_LocalBankThread->start();

    connect(m_LocalBank, SIGNAL(userAdded(QString)), this, SLOT(handleUserAdded(QString)));
    connect(m_LocalBank, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));

    QMetaObject::invokeMethod(m_LocalBank, "init", Qt::QueuedConnection); //init the local bank cache, which also starts connecting to the remote server (long-standing-connection). we send it an event because we don't want to do it in the constructor because that'll mean it'll be done in this thread, which is not what we want

    //gui
    m_Layout = new QVBoxLayout();
    //list widget
    m_AllUsersListWidget = new QListWidget();
    m_AllUsersListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_AllUsersListWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showListContextMenu(QPoint)));
    //context menu
    m_ListContextMenu = new QMenu(this);
    m_AddFundsRequestAction = new QAction("Add Funds", this);
    connect(m_AddFundsRequestAction, SIGNAL(triggered()), this, SLOT(handleAddFundsRequestActionTriggered()));
    m_ListContextMenu->addAction(m_AddFundsRequestAction);

    //1) add user buttan (lineedit as input): creates user account on remote bank
    QHBoxLayout *newUserRow = new QHBoxLayout();
    m_NewUserLineEdit = new QLineEdit();
    m_NewUserButton = new QPushButton("Add User");
    newUserRow->addWidget(m_NewUserLineEdit);
    newUserRow->addWidget(m_NewUserButton);
    m_Layout->addLayout(newUserRow);

    connect(m_NewUserLineEdit, SIGNAL(returnPressed()), this, SLOT(handleNewUserPressed()));
    connect(m_NewUserButton, SIGNAL(clicked()), this, SLOT(handleNewUserPressed()));

    //2) send funds request (highlight existing user first): gets addy to send to, sets up event to be dispatched when funds received at that address that propagates back to us from remote
    //^^^^^initially i was thinking this wouldn't have anything to do with bitcoin. it really doesn't, but if i use bitcoin accounts (which i do plan to. added security), i need to issue the "move" api command. it doesn't access the network or anything, just modifies our wallet.db -- TODO: "move" has minconf=1... what are the implications of this? you can only move an amount that has been confirmed? i guess this is what i want. i'd probably have code checking it too
    //in my secure as fuck version, i'm going to verify a "permission slip" is signed by their private key (using their public decryption cert to decrypt+verify) has the right amount (permission slips can only be used once? do i subtract the amount transferred from the amount the permission slip authorizes? i need to think this out more... but not quite yet). i guess the offline client could keep in sync with a server-sync'd "numTransactions". 0,0... 1,1.. this is the simplest way to make sure a permission slip is used only once. and whatever is left over from the permission slip is simply ignored and remains in thier possession. they need to sign another permission slip to do another transaction. the gui could tell them which # permission slip it expects. if this number increments without their knowledge/doing, they know their shit has been compromised -- TODO: read the previous lol

    //3) purchase something (ad slots) (highlight existing user for 'from'): balance transfer remotely, if sufficient funds. nothing to do with bitcoin. from is highlighted in list, to is (???? maybe a popup form that asks who to give money to? but this is semi-pointless, just use a hardcoded 'to' to start)
    //4) payout (highlight existing user first): asks for btc addy to send to + amount



    m_Layout->addWidget(m_AllUsersListWidget);
    m_Layout->addWidget(m_Debug);
    this->setLayout(m_Layout);
}
mainWebsiteBankView::~mainWebsiteBankView()
{

}
void mainWebsiteBankView::handleNewUserPressed()
{
    //for this prototype, it's good to call this new user
    //but for the actual implementation, this code is going to be munged with the OnLoggedIn slot. we check if our current user's id (Wt user) has a bank account set up at the local app bank cache. if it doesn't (it'll be zero by default), then this is where THIS CODE RIGHT HERE comes into play. for this prototype we're going to NOT have a user db, just a local bank cache db of users and their cached balance. the actual implementation will have a Wt userdb and a local bank cache db associated with every user created (and bank account created only after confirmed via email)

    QString newUser(m_NewUserLineEdit->text().trimmed());
    if(!newUser.isEmpty())
    {
        //m_LocalBank->addUser(newUser);
        QMetaObject::invokeMethod(m_LocalBank, "addUser", Qt::QueuedConnection, Q_ARG(QString, newUser));
    }
}
void mainWebsiteBankView::handleUserAdded(QString newUser)
{
    //this slot gets called after local bank contacts remote bank, adds a user, then remote bank signals that a user has been added, then local bank signals that a user has been added... then we're here.

    new QListWidgetItem(newUser, m_AllUsersListWidget);
    m_Debug->appendPlainText("GUI: " + newUser + " has been add/propagated");
}
void mainWebsiteBankView::showListContextMenu(const QPoint &pos)
{
    m_ListContextMenu->exec(mapToParent(pos));
}
void mainWebsiteBankView::handleAddFundsRequestActionTriggered()
{
    QListWidgetItem *itemRightClicked = m_AllUsersListWidget->currentItem();
    if(itemRightClicked)
    {
        QString user = itemRightClicked->text().trimmed();
        if(!user.isEmpty())
        {
            m_Debug->appendPlainText("requesting an add funds action for user: " + user);
            QMetaObject::invokeMethod(m_LocalBank, "addFundsRequest", Qt::QueuedConnection, Q_ARG(QString, user));

        }
    }
}
