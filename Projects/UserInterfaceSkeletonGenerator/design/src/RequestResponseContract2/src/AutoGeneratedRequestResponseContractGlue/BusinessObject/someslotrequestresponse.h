#ifndef SOMESLOTREQUESTRESPONSE_H
#define SOMESLOTREQUESTRESPONSE_H

#include <QObject>

class BusinessObject;

namespace BusinessObjectRequestResponseContracts
{

class SomeSlotRequestResponse : public QObject
{
    Q_OBJECT
public:
    explicit SomeSlotRequestResponse(BusinessObject *businessClass, QObject *parent = 0);

    void setSuccess(bool success);
    void setXIsEven(bool xIsEven);

    void reset();
private:
    void respond_aka_emitFinishedSignal();
    friend class SomeSlotScopedResponder;

    bool m_Success;
    bool m_XIsEven;
signals: /*private:*/
    void someSlotFinished(bool success, bool xIsEven);
};

}

#endif // SOMESLOTREQUESTRESPONSE_H
