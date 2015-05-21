#ifndef ICLEANROOMSESSION
#define ICLEANROOMSESSION

//#include <QObject>

class CleanRoom;

class ICleanRoomSession/* : public QObject*/
{
    //Q_OBJECT
public:
    ICleanRoomSession();

    //these two api 'returns'
    virtual void handleFinishedQueryingFrontPageView(QList<CleanRoomDoc> frontPageDocs)=0;
    virtual void handleFinishedSubmittingDoc(bool success, QByteArray keyAkaSha1OfDataBody)=0;
    //vs:
    virtual void jumpBackToSessionContextBeforeUpdatingFrontend()=0; //WServer::post, for example

    //I think I need CleanRoomSessionRequests and CleanRoomSessionResponses

protected:
    virtual void respond(ICleanRoomSessionRequest *request, ICleanRoomSessionResponse *response)=0; //respond ensures thread safely getting response to front end
private:
    friend class ICleanRoomSessionRequest;
protected: //signals:
    virtual void getAndSubscribeToFrontPageRequested(ICleanRoomSession cleanRoomSession)=0;
public slots:
    void getAndSubscribeToFrontPage();
//protected: //private slots:
    //virtual void handleFrontPageUpdated(QList<CleanRoomDoc>)=0;
};

#endif // ICLEANROOMSESSION
