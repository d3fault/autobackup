#ifndef IRPCCLIENTSHELPER_H
#define IRPCCLIENTSHELPER_H

#include <QObject>

#include "irpcbusiness.h"

class IRpcClientsHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRpcClientsHelper(IRpcBusiness *business);

    //message dispenser elections
    void setBroadcastDispenserParentForPendingBalanceAddedMessages(QObject *owner);
private:
    IRpcBusiness *m_Business;
signals:

public slots:

};

#endif // IRPCCLIENTSHELPER_H
