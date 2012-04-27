#ifndef OURSERVERFORWTFRONTENDS_H
#define OURSERVERFORWTFRONTENDS_H

#include <QObject>

#include "../shared/ssltcpserver.h"
#include "../shared/WtFrontEndAndAppDbProtocol.h"
#include "Messages/applogicrequest.h"
#include "Messages/applogicrequestresponse.h"

//this class maintains the connections to the wt front ends
//it does not matter from which wt front end an app logic request originates
//nor does it matter to which wt front end an app logic request response [primarily needs to go to].
//this class/thread is where the wt front-ends must 'race' (TODOreq: should _WE_ be the ones to catch the 2nd place entry (say, for puchasing a slot) and rejecting them... or should we just forward the event and let AppLogic do it? I guess it could be either but idk which is optimal and/or better design) to modify the AppDb (AppLogic)
//and this class also pushes all the AppLogic changes out to every connected Wt Front End (TODOop: we could prioritize the front end that actually made the request... for snappier responses etc.. since the others don't even necessarily (we can't know) care for the updates [yet]. this optimization is low priority, but actually sounds worthwhile)
class OurServerForWtFrontEnds : public QObject
{
    Q_OBJECT
public:
    explicit OurServerForWtFrontEnds(QObject *parent = 0);
private:
    SslTcpServer *m_SslTcpServer;
signals:
    void d(const QString &);
    void requestFromWtFrontEnd(AppLogicRequest*);
public slots:
    void startListeningForWtFrontEnds();
    void handleResponseFromAppLogic(AppLogicRequestResponse*); //this somewhat contradicts what i wrote above this class definition in the comments, but it should be noted that for a simple 'GET' request to the app logic, only the wt front-end that requested the information should be sent that information. however, when something in the app logic *CHANGES*, we notify _every_ wt front-end of the change. i suppose my AppLogicRequest[Response] struct can have a bool somethingChanged and also an identifier for the wt front-end that made the request. if the somethingChanged is true, we send to everyone (i guess that's how we can prioritize the wt front-end that made the request). if it's false, we only send to the wt front-end that made the request. it doubles as an optimization when somethingChanged is true.
    //i guess another item in the struct could be a QByteArray that we build using a QDataStream. the bytearray (pointer) is the data itself. whether the changed/updated data or the data that the front-end wants a 'GET' of
private slots:
    void handleClientConnectedAndEncrypted(uint clientId, QSslSocket *client);
    void handleWtFrontEndSentUsData();
};

#endif // OURSERVERFORWTFRONTENDS_H
