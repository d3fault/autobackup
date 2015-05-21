#ifndef ICROSSDOMAINTHREADSAFEAPICALL_H
#define ICROSSDOMAINTHREADSAFEAPICALL_H

class QObject;

class ICrossDomainThreadSafeApiCall
{
public:
    ICrossDomainThreadSafeApiCall(QObject *targetObjectToCallApiOn, const char *apiSlotToCall);
    virtual void returnApiCall(bool lcbOpSuccess, bool dbError, QVariantList optionalResponseParameters = QVariantList())=0;
    const char *apiSlotToCall() const;
private:
    QObject *m_TargetObjectToCallApiOn;
    const char *m_ApiSlotToCall;
};

#endif // ICROSSDOMAINTHREADSAFEAPICALL_H
