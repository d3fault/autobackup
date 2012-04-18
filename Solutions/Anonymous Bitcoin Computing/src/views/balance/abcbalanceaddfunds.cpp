#include "abcbalanceaddfunds.h"

AbcBalanceAddFunds::AbcBalanceAddFunds(WContainerWidget *parent)
    : m_AwaitingValues(false), WContainerWidget(parent)
{
    //Key
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

    this->setLayout(layout);
}
void AbcBalanceAddFunds::handleAddFundsBalanceButtonClicked()
{
    //this is NOT where we want to initially set pending amount, confirmed amount, an existing key, or where we'd want it to tell AppDbHelper to notify us for updates to any of those values. this is only where we'd want to request a NEW key. a specific ACTION on this page.... does NOT happen when the page is loaded. suicide sounds so much nicer.

    //the method (though not THIS method)(that we ALSO call using BLockingQueued) should return an 'AddFundsRequestResult'... which is similar to (but not the same as) the AppDbResult... which is used on every page to notify of new values. the AddFundsRequestResult tells us if we need to wait for pending to become confirmed, use the key already issued... or if we got a new key. my brain fucking hurts especially when i try to factor in all the memory efficiencies of const references and bullshit that will add the fuck up over time.

    //we might also just store the boolean(s) telling us what we need to do within this class... and they could be pushed to us via the notify shits. so we don't need to ask AppDbHelper (except when we actually DO need a new key)... we just ask ourselves

    //if the blocking queued shit doesn't work, just set all the values to "LOADING..." and then do an async request. it might make for a snappier gui too (though 'LOADING...' doesn't help much... AND it's slightly less efficient as now we have to post() the values EVERY request. bah.)


    AppDbResult result;
    QMetaObject::invokeMethod(AppDbHelper::Instance(), "addFundsRequest", Qt::BlockingQueuedConnection, Q_RETURN_ARG(AppDbResult, result), Q_ARG(QString, username), Q_ARG(CallbackInfo, callbackInfo));

    if(result.NotInCacheWeWillNotifyYou)
    {
        WApplication::instance()->deferRendering();
        m_AwaitingValues = true; //the notification updates don't need to do resumeRendering. we only need to resume if we defer here... hence the boolean
    }
    else //it was in the cache so we got our values immediately. in THIS addFundsRequest scenario, that actually probably means we haven't used a key issued to us yet. but in others, it simply means the data was cached.
    {
        processNewValues(result);
    }
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
void AbcBalanceAddFunds::notifyCallback(AppDbResult updateOrResult)
{
    if(m_AwaitingValues)
    {
        WApplication::instance()->resumeRendering();
        m_AwaitingValues = false;
    }
    processNewValues(updateOrResult);
}
void AbcBalanceAddFunds::processNewValues(AppDbResult newValues)
{
    m_AddFundsKeyLineEdit->setText(WtQtUtil::fromQString(newValues.ReturnString1));
    m_PendingPaymentAmountActual->setText(WtQtUtil::fromQString(BtcUtil::doubleToQString(newValues.ReturnDouble1)));
    m_ConfirmedPaymentAmountActual->setText(WtQtUtil::fromQString(BtcUtil::doubleToQString(newValues.ReturnDouble2)));

    WApplication::instance()->triggerUpdate();
}
