//#include <Wt/WServer>
//using namespace Wt;

#include <QObject>
#include <QMetaMethod>

#if 0

class ICrossDomainThreadSafeApiCall // Qt_or_Wt_or_Etc to (request) Qt then back to (response) the same Qt_or_Wt_or_Etc
{
public:
    ICrossDomainThreadSafeApiCall(void *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &slotNameIfQt_Or_wtSessionId, QVariantList optionalResponseParameters = QVariantList())
        : m_ObjectCallingApi(objectCallingApi_aka_Sender)
        , m_ObjectHavingApiCalledOnIt_aka_ReceiverResponder(objectHavingApiCalledOnIt_aka_ReceiverResponder)
        , m_SlotNameIfQt_Or_wtSessionId(slotNameIfQt_Or_wtSessionId)
        , m_OptionalResponseParameters(optionalResponseParameters)
    { }
    virtual void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())=0;
protected:
    void *m_ObjectCallingApi;
    QObject *m_ObjectHavingApiCalledOnIt_aka_ReceiverResponder;
    QString m_SlotNameIfQt_Or_wtSessionId;
    //QVariantList m_ApiCallData;
    QVariantList m_OptionalResponseParameters;
};
class QtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    QtOriginatingCrossDomainThreadSafeApiCall(QObject *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &slotName, QVariantList optionalResponseParameters = QVariantList())
        : ICrossDomainThreadSafeApiCall(objectCallingApi_aka_Sender, objectHavingApiCalledOnIt_aka_ReceiverResponder, slotName, optionalResponseParameters)
    { }
    void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())
    {
        const std::string slotNameStdStr = m_SlotNameIfQt_Or_wtSessionId.toStdString();
        QMetaObject::invokeMethod(static_cast<QObject*>(m_ObjectCallingApi), slotNameStdStr.c_str(), Q_ARG(QVariantList, optionalResponseParameters));
#if 0
        QObject *qobjectToCallApiOf = static_cast<QObject*>(objectToCallApiOf());
        const QMetaObject *metaObject = qobjectToCallApiOf->metaObject();
        QString slotName;
        const std::string slotNameStdStr = slotName.toStdString();
        QMetaMethod metaMethod = metaObject->method(metaObject->indexOfMethod(slotNameStdStr.c_str()));
        metaMethod.invoke(qobjectToCallApiOf, Q_ARG(QVariantList, optionalResponseParameters));
#endif
    }
};
class WApplication;
class WtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    WtOriginatingCrossDomainThreadSafeApiCall(WApplication *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &wtSessionId, QVariantList responseParameters = QVariantList())
        : ICrossDomainThreadSafeApiCall(objectCallingApi_aka_Sender, objectHavingApiCalledOnIt_aka_ReceiverResponder, wtSessionId, responseParameters)
    { }
    void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())
    {
        //Wt::WServer::instance()->post(slotNameIfQt_Or_wtSessionId(), boost::bind(boost::bind(&TheWApplicationInheriter::crossDomainThreadSafeApiCallBack, static_cast<TheWApplicationInheriter*>(objectToCallApiOf()), _1), optionalResponseParameters));
    }
};
int main(int argc, char *argv[])
{
    QVariantList optionalResponseParameters;
    QObject *qtQuerier;
    WApplication *wtQuerier;
    QObject *qobjectWithApiBeingCalled;
    QtOriginatingCrossDomainThreadSafeApiCall constructMePlx(qtQuerier, qobjectWithApiBeingCalled, "retQtSlot", optionalResponseParameters);
    WtOriginatingCrossDomainThreadSafeApiCall constructMeAlsoPlx(wtQuerier, qobjectWithApiBeingCalled, "wtSessionId", optionalResponseParameters);

    //then on the opposite side of things, make the returnApiCall... calls... work
    constructMePlx.doApiCall();
    constructMeAlsoPlx.doApiCall();

    constructMePlx.returnApiCall(true, false, optionalResponseParameters);
    constructMeAlsoPlx.returnApiCall(true, false, optionalResponseParameters);
}
#endif

class ICrossDomainThreadSafeApiCall // Qt_or_Wt_or_Etc to (request) Qt then back to (response) the same Qt_or_Wt_or_Etc
{
public:
    ICrossDomainThreadSafeApiCall(void *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &slotApiCallOnReceiverResponder, const QString &slotCallbackIfQt_Or_wtSessionId, QVariantList optionalResponseParameters = QVariantList())
        : m_ObjectCallingApi(objectCallingApi_aka_Sender)
        , m_ObjectHavingApiCalledOnIt_aka_ReceiverResponder(objectHavingApiCalledOnIt_aka_ReceiverResponder)
        , m_SlotApiCallOnReceiverResponder(slotApiCallOnReceiverResponder)
        , m_SlotCallbackIfQt_Or_wtSessionId(slotCallbackIfQt_Or_wtSessionId)
        , m_OptionalResponseParameters(optionalResponseParameters)
    { }
    virtual void doApiCall()=0;
    virtual void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())=0;
protected:
    void *m_ObjectCallingApi;
    QObject *m_ObjectHavingApiCalledOnIt_aka_ReceiverResponder;
    QString m_SlotApiCallOnReceiverResponder;
    QString m_SlotCallbackIfQt_Or_wtSessionId;
    //QVariantList m_ApiCallData;
    QVariantList m_OptionalResponseParameters;
};
class QtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    QtOriginatingCrossDomainThreadSafeApiCall(QObject *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &slotApiCallOnReceiverResponder, const QString &slotCallback, QVariantList optionalResponseParameters = QVariantList())
        : ICrossDomainThreadSafeApiCall(objectCallingApi_aka_Sender, objectHavingApiCalledOnIt_aka_ReceiverResponder, slotApiCallOnReceiverResponder, slotCallback, optionalResponseParameters)
    { }
    void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())
    {
        const std::string slotNameStdStr = m_SlotCallbackIfQt_Or_wtSessionId.toStdString();
        QMetaObject::invokeMethod(static_cast<QObject*>(m_ObjectCallingApi), slotNameStdStr.c_str(), Q_ARG(QVariantList, optionalResponseParameters));
#if 0
        QObject *qobjectToCallApiOf = static_cast<QObject*>(objectToCallApiOf());
        const QMetaObject *metaObject = qobjectToCallApiOf->metaObject();
        QString slotName;
        const std::string slotNameStdStr = slotName.toStdString();
        QMetaMethod metaMethod = metaObject->method(metaObject->indexOfMethod(slotNameStdStr.c_str()));
        metaMethod.invoke(qobjectToCallApiOf, Q_ARG(QVariantList, optionalResponseParameters));
#endif
    }
};
class WApplication;
class WtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    WtOriginatingCrossDomainThreadSafeApiCall(WApplication *objectCallingApi_aka_Sender, QObject *objectHavingApiCalledOnIt_aka_ReceiverResponder, const QString &slotApiCallOnReceiverResponder, const QString &wtSessionId, QVariantList responseParameters = QVariantList())
        : ICrossDomainThreadSafeApiCall(objectCallingApi_aka_Sender, objectHavingApiCalledOnIt_aka_ReceiverResponder, slotApiCallOnReceiverResponder, wtSessionId, responseParameters)
    { }
    void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())
    {
        //Wt::WServer::instance()->post(slotNameIfQt_Or_wtSessionId(), boost::bind(boost::bind(&TheWApplicationInheriter::crossDomainThreadSafeApiCallBack, static_cast<TheWApplicationInheriter*>(objectToCallApiOf()), _1), optionalResponseParameters));
    }
};
class Business
{
public slots:
    void apiCall0()
    { }
};
class QtOriginatingCrossDomainThreadSafeApiCall_SubmitCleanRoomDoc : public QtOriginatingCrossDomainThreadSafeApiCall
{
public:
    //constructor shit goes here
    void doApiCall()
    {
        //QMetaMethod invoke (api call itself)
        //m_ApiCall.invoke();

        //TODOmb: try to find the overload with matching parameter count+types in requestParameters (do the same for the slot callback (i feel like this means i'm doing something wrong (but maybe it's necessary for my boost/Wt interactions)))

        const std::string slotNameStdStr = m_SlotApiCallOnReceiverResponder.toStdString();
        QVariantList optionalRequestParameters; //TODOreq: should the request parameters be arguments for this method, or constructor arg, or combination of them (property exposure = ?). stick to as needed, add... as needed...
        QMetaObject::invokeMethod(static_cast<QObject*>(m_ObjectCallingApi), slotNameStdStr.c_str(), Q_ARG(QVariantList, optionalRequestParameters));
    }
};
class WtOriginatingCrossDomainThreadSafeApiCall_SubmitCleanRoomDoc : public WtOriginatingCrossDomainThreadSafeApiCall
{
public:
    //constructor shit goes here
    void doApiCall()
    {
        //QMetaMethod invoke (api call itself)
        //m_ApiCall.invoke();
    }
};
int main(int argc, char *argv[])
{
    Business business;


    QVariantList optionalResponseParameters;
    QObject *qtQuerier;
    WApplication *wtQuerier;
    QObject *qobjectWithApiBeingCalled;
    QtOriginatingCrossDomainThreadSafeApiCall_SubmitCleanRoomDoc constructMePlx(qtQuerier, qobjectWithApiBeingCalled, "theApiBeingCalled", "apiCallback", optionalResponseParameters);
    WtOriginatingCrossDomainThreadSafeApiCall_SubmitCleanRoomDoc constructMeAlsoPlx(wtQuerier, qobjectWithApiBeingCalled, "theApiBeingCalled", "wtSessionId", optionalResponseParameters);

    //then on the opposite side of things, make the returnApiCall... calls... work
    constructMePlx.doApiCall();
    constructMeAlsoPlx.doApiCall();

    constructMePlx.returnApiCall(true, false, optionalResponseParameters);
    constructMeAlsoPlx.returnApiCall(true, false, optionalResponseParameters);
}
