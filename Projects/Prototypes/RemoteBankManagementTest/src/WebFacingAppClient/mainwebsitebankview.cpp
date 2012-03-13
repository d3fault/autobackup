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

    m_AppLogicThread = new QThread();
    m_AppLogic = new AdAgencyAppLogic();
    m_AppLogic->moveToThread(m_AppLogicThread);
    m_AppLogicThread->start();

    connect(m_AppLogic, SIGNAL(campaignAdded(QString)), this, SLOT(handleCampaignAdded(QString)));
    connect(m_AppLogic, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));

    QMetaObject::invokeMethod(m_AppLogic, "init", Qt::QueuedConnection);

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
    //....2 is done, i kept thinking it was the purchase slot... but that's #3. 2 is the right-click->add funds functionality

    //3) purchase something (ad slots) (highlight existing user for 'from'): balance transfer remotely, if sufficient funds. nothing to do with bitcoin. from is highlighted in list, to is (???? maybe a popup form that asks who to give money to? but this is semi-pointless, just use a hardcoded 'to' to start)
    //^^^^^initially i was thinking this wouldn't have anything to do with bitcoin. it really doesn't, but if i use bitcoin accounts (which i do plan to. added security), i need to issue the "move" api command. it doesn't access the network or anything, just modifies our wallet.db -- TODO: "move" has minconf=1... what are the implications of this? you can only move an amount that has been confirmed? i guess this is what i want. i'd probably have code checking it too
    //in my secure as fuck version, i'm going to verify a "permission slip" is signed by their private key (using their public decryption cert to decrypt+verify) has the right amount (permission slips can only be used once? do i subtract the amount transferred from the amount the permission slip authorizes? i need to think this out more... but not quite yet). i guess the offline client could keep in sync with a server-sync'd "numTransactions". 0,0... 1,1.. this is the simplest way to make sure a permission slip is used only once. and whatever is left over from the permission slip is simply ignored and remains in thier possession. they need to sign another permission slip to do another transaction. the gui could tell them which # permission slip it expects. if this number increments without their knowledge/doing, they know their shit has been compromised -- TODO: read the previous lol
    //ok so in order to do a "balance transfer", we need to add a user db. this is not the same as the userbankaccount db. the user db will only be on the web facing cache. it will contain a list of users, their passwords, email addresses... and VITAL: their list of "slots" that can be purchased. each slot has more than one slot, so slot is a terrible word to use. "Website Owner Ad Block" is better.. which each day or whatever being a "slot". i think i do need to keep a list of the slots and what they sold for, so i can double the next day and also because we'll be multiple days ahead of the one being currently shown. each user can have multiple "website owner ad blocks" corresponding to multiple websites. this contradicts what i've written in handleNewUserPressed about not having a user db. i need one in order to do #2: send funds / balance transfer / slot purchase (i should also add more like: "add website owner block", which allows the user to specify a minimum (1 btc by default?))
    //...3... they select a user to be the owner of the ad owner ad campaign, type in a name for the campaign, click create ad campaign. it adds it to the list (and remembers the owner). click a new user (to be the slot purchaser) and click the created ad campaign and then click "purchase slot". they must have the proper amount of funds in order to purchase the slot. the balance is transferred to the owner of the ad slot and the ad slot is remembered and we do the math of doubling and counting down etc eventually... can just be a set price at first
    QHBoxLayout *newAdCampaignRow = new QHBoxLayout();
    m_NewAdOwnerAdCampaignLineEdit = new QLineEdit();
    m_NewAdOwnerAdCampaignButton = new QPushButton("Create Ad Campaign");
    newAdCampaignRow->addWidget(m_NewAdOwnerAdCampaignLineEdit);
    newAdCampaignRow->addWidget(m_NewAdOwnerAdCampaignButton);
    m_Layout->addLayout(newAdCampaignRow);

    connect(m_NewAdOwnerAdCampaignLineEdit, SIGNAL(returnPressed()), this, SLOT(handleNewAdCampaignPressed())); //TODO: maybe send a signal directly to the app logic (and for add user above, directly to the bank server. this represents the gui, and in this case, we'd be sending an event to the server, doing C++ logic, then going back to the gui, only to then send another signal to the server. or something. idfk. you get the point though... handleNewUserPressed() probably doesn't belong in this class... since this is technically a gui class. nor does this handleNewAdCampaignPressed... but fuck it.
    connect(m_NewAdOwnerAdCampaignButton, SIGNAL(clicked()), this, SLOT(handleNewAdCampaignPressed())); //i might be wrong with the above though. maybe some js checking of values could be done which could then send a JSignal or whatever to the server (which has to do the same checks again... CAN NEVER TRUST A GUI/user... <<<---- TODO

    m_AllAdOwnerAdCampaignsListWidget = new QListWidget();


    //4) payout (highlight existing user first): asks for btc addy to send to + amount



    m_Layout->addWidget(m_AllUsersListWidget);
    m_Layout->addWidget(m_AllAdOwnerAdCampaignsListWidget);
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
    //todo: see 2) send funds request above, 4th paragraph down

    QString newUser(m_NewUserLineEdit->text().trimmed());
    if(!newUser.isEmpty())
    {
        //TODO: for this prototype, we should also call our AddUser on our AppLogic db  here too. the impl will do this at a different time... and the server AddUser only called after confirmed + first log in
        QMetaObject::invokeMethod(m_AppLogic, "addUser", Qt::BlockingQueuedConnection, Q_ARG(QString, newUser)); //BlockingQueued because we don't want race to create it on the server before we create it locally. it should really check that it was created successfully before creating on the server... but the impl will do these addUsers at completely different times so meh. we could probably do QueuedConnection without bad results... but i don't even want my brain to contemplate that codepath... so BlockingQueued it is

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
    QString user = getCurrentlySelectedUsername();
    if(!user.isEmpty())
    {
        m_Debug->appendPlainText("requesting an add funds action for user: " + user);
        QMetaObject::invokeMethod(m_LocalBank, "addFundsRequest", Qt::QueuedConnection, Q_ARG(QString, user));
    }
    //else: should never get to else, since this is only triggered on a right-click on the list widget... implies selecting.
}
void mainWebsiteBankView::handleNewAdCampaignPressed()
{
    QString newAdCampaign(m_NewAdOwnerAdCampaignLineEdit->text().trimmed());
    if(!newAdCampaign.isEmpty())
    {
        QString ownerOfAdCampaign = getCurrentlySelectedUsername();
        if(!ownerOfAdCampaign.isEmpty())
        {
            QMetaObject::invokeMethod(m_AppLogic, "createAdCampaignXForUserY", Qt::QueuedConnection, Q_ARG(QString, newAdCampaign), Q_ARG(QString, ownerOfAdCampaign));
        }
        else
        {
            m_Debug->appendPlainText("you need to select a valid username to be the owner of the ad campaign");
        }
    }
    else
    {
        m_Debug->appendPlainText("you need to type in a name for the new ad campaign");
    }
}
QString mainWebsiteBankView::getCurrentlySelectedUsername()
{
    QListWidgetItem *selectedUsernameListWidgetItem = m_AllUsersListWidget->currentItem();
    if(selectedUsernameListWidgetItem)
    {
        return selectedUsernameListWidgetItem->text().trimmed();
    }
    else
    {
        m_Debug->appendPlainText("you need to select a user for that action");
        return QString(); //callers have to verify a non-empty string still
    }
}
void mainWebsiteBankView::handleCampaignAdded(QString newCampaignName)
{
    new QListWidgetItem(newCampaignName, m_AllAdOwnerAdCampaignsListWidget);
}
