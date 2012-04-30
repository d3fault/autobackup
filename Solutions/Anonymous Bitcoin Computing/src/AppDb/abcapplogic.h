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

    //App Logic -- these methods get called in AppLogicRequest.processAppLogicRequest after the AppLogicRequest is emit'd over to us........... and maybe some of these App Logic methods might be called by some parallel that interfaces with the Bitcoin Server (TODOTODOTODOreq)
    //void createBankAccountForUser(const QString &user, AppLogicRequestResponse *responseThatWeFillOutAndEmitBack);
private:
    QList<BankServerActionRequest*> m_ListOfPendingBankRequests;
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
