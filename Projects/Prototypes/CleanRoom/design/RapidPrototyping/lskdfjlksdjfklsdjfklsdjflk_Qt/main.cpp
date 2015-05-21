#include <QCoreApplication>

#include <QVariant>
#include <boost/function.hpp>

class IGetCouchbaseDocumentByKeyRequestResponder
{
public:
    virtual respond(void *objectToRespondTo_andOrAka_userData, const QString &slotNameIfQt_Or_WtSessionId, const QString &couchbaseGetKeyInput /*keyUsedAsArgForGetRequestInDb*/, const QString &couchbaseGetValueOutput, bool lcbOpSuccess, bool dbError)=0;
};
void AbcGuiGetCouchbaseDocumentByKeyRequestResponder::respond(const std::string &wtSessionId, void *wApplicationPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    Wt::WServer::instance()->post(wtSessionId, boost::bind(boost::bind(&TheWApplicationInheriter::getCouchbaseDocumentByKeyFinished, static_cast<TheWApplicationInheriter*>(wApplicationPointerForCallback), _1, _2, _3, _4), couchbaseGetKeyInput, couchbaseDocument, lcbOpSuccess, dbError));
}
void AbcApiGetCouchbaseDocumentByKeyRequestResponder::respond(const std::string &wtSessionId_UNUSED, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    (void)wtSessionId_UNUSED;
    QMetaObject::invokeMethod(static_cast<QObject*>(getTodaysAdSlotServerClientConnection), "backendDbGetCallback", Qt::QueuedConnection, Q_ARG(std::string, couchbaseDocument), Q_ARG(bool, lcbOpSuccess), Q_ARG(bool, dbError));
}


class CleanRoomSessionRequest
{
    TheRequestAsAMetaMethodToCallOnceOnBusinessThreadWhichIsEasyUsingQMetaInvoke request;
    QString couchbaseSetKeyInput;
    QString couchbaseSetValueInput;

    //"how to respond" details + args to use
#if 0
    void *m_ObjectToRespondTo_andOrAka_userData; //QObject or WApplication pointer (pure virtuals guarantees us one of their code paths is available)
    QString m_SlotNameIfQt_Or_WtSessionId;
    bool m_LcbOpSuccess;
    bool m_DbError;
#endif

    QVariantList responseData;
    //responseData.append(QVariant::fromValue(static_cast<void*>(NULL)));

    //Qt
    QObject *targetQObject;
    QString slotName;
    responseData.append(QVariant::fromValue(static_cast<void*>(targetQObject)));
    responseData.append(QVariant(slotName));

    //or

    //Wt
    WApplication *wApplicationPointer;
    QString wtSessionId;
    responseData.append(QVariant::fromValue(static_cast<void*>(wApplicationPointer))); //Wt
    responseData.append(QVariant(wtSessionId));

    //bool lcbOpSuccess is the 3rd QVariant in the QVariantList responseData, bool dbError is 4th. Maybe I should allocate an index for CAS, I guess 5th is most logical
};

class ICleanRoomSessionRequest
{
    ICleanRoomSessionRequest(QVariantList requestAndResponseDetails)
        : m_RequestAndResponseDetails(requestAndResponseDetails)
    { }
    QVariantList m_RequestAndResponseDetails;

    ICleanRoomSessionRequest(/* input details:*/ requestAsMetaMethod, bool key, /* output details:*/ void *pointerToRequester, const QString &slotName_Or_WtSessionId)
    {
        m_RequestAndResponseDetails.append(requestAsMetaMethod);
        m_RequestAndResponseDetails.append(QVariant::fromValue(static_cast<void*>(pointerToRequester)));
        m_RequestAndResponseDetails.append(slotName_Or_WtSessionId);
    }
    respond(ICleanRoomSessionRequest)
};

typedef CleanRoomSessionRequest DataType;

void processWt(DataType data)
{
    //Wt::WServer::post(data->sessionId(), )
    Wt::WServer::instance()->post(data->sessionId(), boost::bind(boost::bind(&TheWApplicationInheriter::methodToCallToNotifyOfAResponse, static_cast<TheWApplicationInheriter*>(pointerForCallback), _1), firstArgVariable));
}
void processGui(QDataType data)
{

}









class ICrossDomainThreadSafeApiCall // Qt_or_Wt_or_Etc to (request) Qt then back to (response) the same Qt_or_Wt_or_Etc
{
public:
    ICrossDomainThreadSafeApiCall(void *objectToCallApiOf, const QString &slotNameIfQt_Or_wtSessionId)
        : m_ObjectToCallApiOf(objectToCallApiOf)
        , m_SlotNameIfQt_Or_wtSessionId(slotNameIfQt_Or_wtSessionId)
    { }
    ICrossDomainThreadSafeApiCall(void *objectToCallApiOf, const QString &slotNameIfQt_Or_wtSessionId, QVariantList responseParameters)
        : m_ObjectToCallApiOf(objectToCallApiOf)
        , m_SlotNameIfQt_Or_wtSessionId(slotNameIfQt_Or_wtSessionId)
        , m_ApiCallData(responseParameters)
    { }
    void *objectToCallApiOf() const
    {
        return m_ObjectToCallApiOf;
    }
    const QString &slotNameIfQt_Or_wtSessionId() const
    {
        return m_SlotNameIfQt_Or_wtSessionId;
    }
    QVariantList responseParameters() const
    {
        return m_ResponseParameters; //can be populated with return arg, cas, lcbOpError, and/or dbError
    }
    virtual void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())=0;
private:
    //QVariantList m_ApiCallData;
    QVariantList m_ResponseParameters;
};

int main(int argc, char *argv[])
{
    QVariantList optionalResponseParameters;
    QtOriginatingCrossDomainThreadSafeApiCall constructMePlx(optionalResponseParameters);
    WtOriginatingCrossDomainThreadSafeApiCall constructMeAlsoPlx(optionalResponseParameters);

    //then on the opposite side of things, make the returnApiCall... calls... work

    constructMePlx.returnApiCall(true, false, optionalResponseParameters);
    constructMeAlsoPlx.returnApiCall(true, false, optionalResponseParameters);
}

class QtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    QtOriginatingCrossDomainThreadSafeApiCall(QObject *objectToCallApiOf, const QString &slotName)
        : ICrossDomainThreadSafeApiCall(objectToCallApiOf, slotName)
    { }
    QtOriginatingCrossDomainThreadSafeApiCall(void *objectToCallApiOf, const QString &slotName, QVariantList responseParameters)
        : ICrossDomainThreadSafeApiCall(objectToCallApiOf, slotName, responseParameters)
    { }
    void returnApiCall(QVariantList optionalResponseParameters = QVariantList())
    {
        QMetaObject::invokeMethod(static_cast<QObject*>(objectToCallApiOf()), slotNameIfQt_Or_wtSessionId() /*"crossDomainThreadSafeApiCallBack"*/, Q_ARG(QVariantList, optionalResponseParameters));
    }
};
class WtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    WtOriginatingCrossDomainThreadSafeApiCall(WApplication *objectToCallApiOf, const QString &wtSessionId)
        : ICrossDomainThreadSafeApiCall(objectToCallApiOf, wtSessionId)
    { }
    ICrossDomainThreadSafeApiCall(void *objectToCallApiOf, const QString &wtSessionId, QVariantList responseParameters)
        : ICrossDomainThreadSafeApiCall(objectToCallApiOf, wtSessionId, responseParameters)
    { }
    void returnApiCall(QVariantList optionalResponseParameters = QVariantList())
    {
        Wt::WServer::instance()->post(slotNameIfQt_Or_wtSessionId(), boost::bind(boost::bind(&TheWApplicationInheriter::crossDomainThreadSafeApiCallBack, static_cast<TheWApplicationInheriter*>(objectToCallApiOf()), _1), optionalResponseParameters));
    }
}








int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DataType data;
    processWt(data);
    processGui(data);

    return a.exec();
}

class IRespond
{
    void respond(void *objectToRespondTo_andOrAka_userData, const QString &slotNameIfQt_Or_WtSessionId, const QString &couchbaseGetKeyInput /*keyUsedAsArgForGetRequestInDb*/, const QString &couchbaseGetValueOutput, bool lcbOpSuccess, bool dbError)=0;
};
