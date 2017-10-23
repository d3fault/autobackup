#ifndef SOMESLOTREQUESTRESPONSE_H
#define SOMESLOTREQUESTRESPONSE_H

#include <QObject>

class BusinessObject;

class SomeSlotRequestResponse : public QObject
{
    Q_OBJECT
public:
    explicit SomeSlotRequestResponse(BusinessObject *businessClass, QObject *parent = 0);
    void setSuccess(bool success);
    bool setXIsEven(bool xIsEven);
private:
    void respond_aka_emitFinishedSignal();
    friend class SomeSlotScopedResponder;

    bool m_Success;
    bool m_XIsEven;
/*private:*/ signals:
    void someSlotFinished(bool success, bool xIsEven);
};

#endif // SOMESLOTREQUESTRESPONSE_H
