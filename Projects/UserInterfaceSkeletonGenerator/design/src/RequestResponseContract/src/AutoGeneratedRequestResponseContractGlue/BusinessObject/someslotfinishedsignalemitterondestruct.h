#ifndef SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H
#define SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H

#include <QObject>

#include <memory>

namespace BusinessObjectRequestResponseContract
{

class SomeSlotRequestResponse;

class SomeSlotFinishedSignalEmitterOnDestruct : public QObject
{
    Q_OBJECT
public:
    explicit SomeSlotFinishedSignalEmitterOnDestruct();
    void setSuccess(bool success);
    void setXIsEven(bool xIsEven);
    ~SomeSlotFinishedSignalEmitterOnDestruct();
private:    
    bool m_Success;
    bool m_XIsEven;
signals:
    void signalToBeEmittedInDestructor(bool success, bool xIsEven);
};

class SomeSlotScopedResponder : public std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>
{
public:
    SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent);
    void deferResponding();
private:
    SomeSlotRequestResponse *m_Parent;
};

}

#endif // SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H
