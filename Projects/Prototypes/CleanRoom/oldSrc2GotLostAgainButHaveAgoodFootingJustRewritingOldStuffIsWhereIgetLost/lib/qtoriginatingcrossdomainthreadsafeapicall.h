#ifndef QTORIGINATINGCROSSDOMAINTHREADSAFEAPICALL_H
#define QTORIGINATINGCROSSDOMAINTHREADSAFEAPICALL_H

#include "icrossdomainthreadsafeapicall.h"

#include <QVariantList>

class CleanRoomSession;

class QtOriginatingCrossDomainThreadSafeApiCall : public ICrossDomainThreadSafeApiCall
{
public:
    QtOriginatingCrossDomainThreadSafeApiCall(CleanRoomSession *session, const char *slotCallback);
    void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList());
private:
    CleanRoomSession *m_Session;
    const char *m_SlotCallback;
};

#endif // QTORIGINATINGCROSSDOMAINTHREADSAFEAPICALL_H
