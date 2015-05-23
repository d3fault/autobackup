#include "apicall.h"

ApiCall::ApiCall(Api api, const QString &apiCallSlotName, const QList<ApiCallArg> &requestArgs, const QList<ApiCallArg> &responseArgs)
    : ParentApi(api)
    , ApiCallSlotName(apiCallSlotName)
    , RequestArgs(requestArgs)
    , ResponseArgs(responseArgs)
{ }
