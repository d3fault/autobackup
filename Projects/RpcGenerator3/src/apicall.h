#ifndef APICALL_H
#define APICALL_H

#include "api.h"

struct ApiCall
{
    ApiCall(Api *parentApi, const QString &apiCallSlotName, const QList<ApiCallArg> &requestArgs = QList<ApiCallArg>(), const QList<ApiCallArg> &responseArgs = QList<ApiCallArg>());
    Api *ParentApi;
    QString ApiCallSlotName;
    QList<ApiCallArg> RequestArgs;
    QList<ApiCallArg> ResponseArgs;
};

#endif // APICALL_H
