#include "addoublesrawmathlaunchsim.h"

AdDoublesRawMathLaunchSim::AdDoublesRawMathLaunchSim(QObject *parent) :
    QObject(parent), m_CurrentSlotIndexOnDisplay(-1), m_CurrentSlotIndexForSale(0), m_MinPrice(0.00000001), m_SlotFillerLengthHours(1)
{
    m_OneSecondTimer.setInterval(1000);
    connect(&m_OneSecondTimer, SIGNAL(timeout()), this, SLOT(oneSecondTimerTimedOut()));
}
void AdDoublesRawMathLaunchSim::buyAtPrice(double priceToBuyAt)
{
    SlotFillerPod *newSlotFiller = new SlotFillerPod();
    newSlotFiller->PurchasePrice = priceToBuyAt;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    newSlotFiller->PurchaseTimestamp = currentDateTime;

    ++m_CurrentSlotIndexForSale;
    emit slotFillerForSaleChanged(m_CurrentSlotIndexForSale);

    if(m_CurrentSlotIndexOnDisplay == -1)
    {
        //first, so use currentTime and make it current slot, and also start the timer
        newSlotFiller->StartTimestamp = currentDateTime;
        ++m_CurrentSlotIndexOnDisplay;
        emit slotFillerOnDisplayChanged(m_CurrentSlotIndexOnDisplay);
        m_OneSecondTimer.start();
    }
    else
    {
        QDateTime latestSlotFillerExpireDateTime = m_AllSlotFillers.last()->StartTimestamp;
        latestSlotFillerExpireDateTime = latestSlotFillerExpireDateTime.addSecs(m_SlotFillerLengthHours*60*60);
        if(currentDateTime >= latestSlotFillerExpireDateTime) //TODOoptimization: merge expired calculatioin in getCurrentPrice
        {
            //expired, so use currentTime and make it current slot. we should also start the timer back up
            newSlotFiller->StartTimestamp = currentDateTime;
            ++m_CurrentSlotIndexOnDisplay;
            emit slotFillerOnDisplayChanged(m_CurrentSlotIndexOnDisplay);
            m_OneSecondTimer.start();
        }
        else
        {
            //not expired, so use it's expiration date as our start time. this one does NOT emit slotFillerChange. we have to wait for current to expire first

            newSlotFiller->StartTimestamp = latestSlotFillerExpireDateTime;
        }
    }

    //TODOreq: delete
    m_AllSlotFillers.append(newSlotFiller);
}
double AdDoublesRawMathLaunchSim::getCurrentPrice()
{
    if(m_CurrentSlotIndexOnDisplay == -1)
    {
        //no purchases yet, minPrice
        return m_MinPrice;
    }

    //we know we have at least one
    QDateTime currentDateTime = QDateTime::currentDateTime();
    SlotFillerPod *latestSlotFiller = m_AllSlotFillers.last();

    QDateTime latestSlotFillerExpireDateTime = latestSlotFiller->StartTimestamp;
    latestSlotFillerExpireDateTime = latestSlotFillerExpireDateTime.addSecs(m_SlotFillerLengthHours*60*60);
    if(currentDateTime >= latestSlotFillerExpireDateTime)
    {
        //the last one expired, so we're back at minPrice again
        return m_MinPrice;
    }

    //we need to calculate it using maths
    double latestSlotFillerExpireDateTimeDouble = latestSlotFillerExpireDateTime.toMSecsSinceEpoch()/1000.0;
    double latestSlotFillerPurchaseTime = latestSlotFiller->PurchaseTimestamp.toMSecsSinceEpoch()/1000.0;
    double m = ((m_MinPrice-(latestSlotFiller->PurchasePrice*2))/(latestSlotFillerExpireDateTimeDouble-latestSlotFillerPurchaseTime)); //well that was easy...
    double b = m_MinPrice - (m * latestSlotFillerExpireDateTimeDouble);
    double currentPrice = (m*(currentDateTime.toMSecsSinceEpoch()/1000))+b; //fuck yea high school (DON'T DROP OUT KIDS! (jk college is a scam. drop out after high school. that is to say, just consider yourself finished and never go to college))
    return currentPrice;
}
void AdDoublesRawMathLaunchSim::launch(double minPrice, unsigned int slotFillerLengthHours)
{
    m_MinPrice = minPrice;
    m_SlotFillerLengthHours = slotFillerLengthHours;

    //initially, nothing to do. We hover at the minPrice until someone buys it.
    //emit d("Buy this ad for: " + QString::number(minPrice, 'f', 8));
    //emit slotAvailableAtPrice(m_CurrentSlotIndexForSale, minPrice);
    emit slotFillerForSaleChanged(m_CurrentSlotIndexForSale);

    if(!m_OneSecondTimer.isActive())
    {
        m_OneSecondTimer.start();
    }
}
void AdDoublesRawMathLaunchSim::buyAttempt(unsigned int slotFillerNum, double priceToTryToBuyAt)
{
    if(slotFillerNum == m_CurrentSlotIndexForSale)
    {
        if(priceToTryToBuyAt >= getCurrentPrice()) //allow overpayment, because latency and stupidity exist
        {
            buyAtPrice(priceToTryToBuyAt);
        }
    }
}
void AdDoublesRawMathLaunchSim::oneSecondTimerTimedOut()
{
    emit slotAvailableAtPrice(m_CurrentSlotIndexForSale, getCurrentPrice());




    //eh this doesn't exactly belong here, but i need to put it somewhere. the final implementation should probably use cron or something (or it's own dedicated timer i suppose). basically i need to switch to the next slot (if one exists (TODOreq: handle case where one doesn't exist (if not in this prototype, definitely in actual))) once the current one expires...

    if(m_CurrentSlotIndexOnDisplay == -1)
    {
        //no purchases yet, so nothing to even check... also stop timer until first purchase is made
        m_OneSecondTimer.stop();
        return;
    }

    QDateTime currentSlotFillerExpireDateTime = m_AllSlotFillers.at(m_CurrentSlotIndexOnDisplay)->StartTimestamp;
    currentSlotFillerExpireDateTime = currentSlotFillerExpireDateTime.addSecs(m_SlotFillerLengthHours*60*60);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if(currentDateTime >= currentSlotFillerExpireDateTime)
    {
        //expired, but that doesn't mean we have a 'next'. if we don't we should stop the timer

        if(m_AllSlotFillers.size() > (m_CurrentSlotIndexOnDisplay+1))
        {
            //we have a next
            ++m_CurrentSlotIndexOnDisplay;
            emit slotFillerOnDisplayChanged(m_CurrentSlotIndexOnDisplay);
        }
        else
        {
            //no next
            m_OneSecondTimer.stop();
        }
    }
    //not expired, dgaf
}
void AdDoublesRawMathLaunchSim::stop()
{
    //eh just stop timer if active and i guess reset shit and close down?
    if(m_OneSecondTimer.isActive())
    {
        m_OneSecondTimer.stop();
    }
    m_CurrentSlotIndexForSale = 0;
    m_CurrentSlotIndexOnDisplay = -1;

    QListIterator<SlotFillerPod*> it(m_AllSlotFillers);
    while(it.hasNext())
    {
        SlotFillerPod *currentSlotFiller = it.next();
        delete currentSlotFiller;
    }
    m_AllSlotFillers.clear();
}
