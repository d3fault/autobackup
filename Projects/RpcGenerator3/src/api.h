#ifndef API
#define API

#include <QString>

struct ApiCallArg
{
    ApiCallArg(QString apiCallArgType, QString apiCallArgName)
        : ApiCallArgType(apiCallArgType)
        , ApiCallArgName(apiCallArgName)
    { }
    QString ApiCallArgType;
    QString ApiCallArgName;
};
struct ApiCall
{
    ApiCall(const QString &apiCallSlotName, const QList<ApiCallArg> &requestArgs = QList<ApiCallArg>(), const QList<ApiCallArg> &responseArgs = QList<ApiCallArg>())
        : ApiCallSlotName(apiCallSlotName)
        , RequestArgs(requestArgs)
        , ResponseArgs(responseArgs)
    { }
    QString ApiCallSlotName;
    QList<ApiCallArg> RequestArgs;
    QList<ApiCallArg> ResponseArgs;
};
struct Api
{
    Api(QString apiName)
        : ApiName(apiName)
    { }
    QString ApiName;
    QList<ApiCall> ApiCalls;
    void createApiCall(const QString &apiCallSlotName, const QList<ApiCallArg> &requestArgs = QList<ApiCallArg>(), const QList<ApiCallArg> &responseArgs = QList<ApiCallArg>())
    {
        ApiCall apiCall(apiCallSlotName, requestArgs, responseArgs);
        ApiCalls.append(apiCall);
    }
};

#endif // API

