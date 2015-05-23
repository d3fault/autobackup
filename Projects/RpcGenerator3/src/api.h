#ifndef API
#define API

#include <QString>

#include "apicall.h"

struct ApiCallArg
{
    ApiCallArg(QString apiCallArgType, QString apiCallArgName)
        : ApiCallArgType(apiCallArgType)
        , ApiCallArgName(apiCallArgName)
    { }
    QString ApiCallArgType;
    QString ApiCallArgName;
};
struct Api
{
    Api(QString apiName = QString("Unnamed Api"))
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

