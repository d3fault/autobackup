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
class Api;
struct ApiCall
{
    ApiCall(Api *api, const QString &apiCallSlotName, const QList<ApiCallArg> &requestArgs = QList<ApiCallArg>(), const QList<ApiCallArg> &responseArgs = QList<ApiCallArg>())
        : ParentApi(api)
        , ApiCallSlotName(apiCallSlotName)
        , RequestArgs(requestArgs)
        , ResponseArgs(responseArgs)
    { }
    Api *ParentApi;
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
        ApiCall apiCall(this, apiCallSlotName, requestArgs, responseArgs);
        ApiCalls.append(apiCall);
    }
};

#endif // API

