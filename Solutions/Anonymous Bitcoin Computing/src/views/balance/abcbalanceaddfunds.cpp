#include "abcbalanceaddfunds.h"

AbcBalanceAddFunds::AbcBalanceAddFunds(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WLabel("Hello Add Funds"));
    /*//Key
    WLabel *addFundsKeyLabel = new WLabel("Key: ");
    m_AddFundsKeyLineEdit = new WLineEdit("[None]");
    m_AddFundsKeyLineEdit->setReadOnly(true);
    addFundsKeyLabel->setBuddy(m_AddFundsKeyLineEdit);


    //Pending Payment Amount
    WLabel *pendingPaymentAmountLabel = new WLabel("Pending Amount: ");
    m_PendingPaymentAmountActual = new WLabel("0.00000000");

    //Confirmed Payment Amount
    WLabel *confirmedPaymentAmountLabel = new WLabel("Confirmed Amount: ");
    m_ConfirmedPaymentAmountActual = new WLabel("0.00000000");

    //Request Add Funds Key Pushbutton
    WPushButton *requestAddFundsButton = new WPushButton("Request Add Funds Key");
    requestAddFundsButton->clicked().connect(this, &AbcBalanceAddFunds::handleAddFundsBalanceButtonClicked);


    //Layout
    WGridLayout *layout = new WGridLayout();

    layout->addWidget(addFundsKeyLabel, 0, 0);
    layout->addWidget(m_AddFundsKeyLineEdit, 0, 1);

    layout->addWidget(pendingPaymentAmountLabel, 1, 0);
    layout->addWidget(m_PendingPaymentAmountActual, 1, 1);

    layout->addWidget(confirmedPaymentAmountLabel, 2, 0);
    layout->addWidget(confirmedPaymentAmountLabel, 2, 1);

    layout->addWidget(requestAddFundsButton, 3, 1);

    this->setLayout(layout);*/
}
void AbcBalanceAddFunds::handleAddFundsBalanceButtonClicked()
{
    /*
    WApplication::instance()->deferRendering(); //freeze the UI until we get our response
    QMetaObject::invokeMethod(AbcAppDbHelper::Instance(), "addFundsRequest", Qt::QueuedConnection, Q_ARG(QString,username)); //maybe also send in our Wt SessionID and a Callback?
    */
}


const std::string AbcBalanceAddFunds::ReadableText = "Add Funds";
const std::string AbcBalanceAddFunds::PreferredInternalPath = "/balance/add-funds";

bool AbcBalanceAddFunds::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "/balance/add")
        return true;
    if(internalPath == "/balance/addfunds")
        return true;
    return false;
}
bool AbcBalanceAddFunds::requiresLogin()
{
    return true;
}
