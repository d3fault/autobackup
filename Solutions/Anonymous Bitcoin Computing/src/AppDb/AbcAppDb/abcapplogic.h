#ifndef APPLOGIC_H
#define APPLOGIC_H

#include <QObject>

#include "Messages/applogicrequest.h"
#include "Messages/applogicrequestresponse.h"
#include "Messages/bankserveractionrequest.h"
#include "Messages/bankserveractionrequestresponse.h"

class AbcAppLogic : public QObject
{
    Q_OBJECT
public:
    explicit AbcAppLogic(QObject *parent = 0);

signals:
    void d(const QString &);
    void appLogicRequestRequiresBankServerAction(BankServerActionRequest*);
    void responseToWtFrontEndReady(AppLogicRequestResponse*);
public slots:
    void init();
    void handleRequestFromWtFrontEnd(AppLogicRequest*);
    void handleResponseFromBankServer(BankServerActionRequestResponse*);
};

#endif // APPLOGIC_H
