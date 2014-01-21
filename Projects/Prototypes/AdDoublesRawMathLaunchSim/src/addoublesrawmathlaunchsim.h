#ifndef ADDOUBLESRAWMATHLAUNCHSIM_H
#define ADDOUBLESRAWMATHLAUNCHSIM_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QTimer>
#include <QListIterator>

#include "slotfillerpod.h"

class AdDoublesRawMathLaunchSim : public QObject
{
    Q_OBJECT
public:
    explicit AdDoublesRawMathLaunchSim(QObject *parent = 0);
private:
    int m_CurrentSlotIndexOnDisplay;
    unsigned int m_CurrentSlotIndexForSale;
    double m_MinPrice;
    unsigned int m_SlotFillerLengthHours;

    QList<SlotFillerPod*> m_AllSlotFillers;

    void buyAtPrice(double priceToBuyAt);

    double getCurrentPrice();

    QTimer m_OneSecondTimer; //TODOoptional: instead of sending the current price every 1 second, we should only send the front end the math required to do the calculations itself... and only send them the numbers (m,b,etc) when they change. I definitely want this in ABC, but in this prototype it doesn't matter and KISS for now
signals:
    void slotAvailableAtPrice(unsigned int slotFillerNum, double priceAvailableAt);
    void slotFillerOnDisplayChanged(int newSlotFillerOnDisplayIndex);
    void slotFillerForSaleChanged(unsigned int newSlotFillerForSaleIndex);
public slots:
    void launch(double minPrice, unsigned int slotFillerLengthHours);
    void buyAttempt(unsigned int slotFillerNum, double priceToTryToBuyAt);
private slots:
    void oneSecondTimerTimedOut();
    void stop();
};

#endif // ADDOUBLESRAWMATHLAUNCHSIM_H
