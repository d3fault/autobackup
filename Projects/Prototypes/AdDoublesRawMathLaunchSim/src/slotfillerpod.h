#ifndef SLOTFILLERPOD_H
#define SLOTFILLERPOD_H

#include <QDateTime>

struct SlotFillerPod
{
    QDateTime PurchaseTimestamp;
    QDateTime StartTimestamp;
    double PurchasePrice;
};

#endif // SLOTFILLERPOD_H
