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
    explicit SomeSlotFinishedSignalEmitterOnDestruct(bool emitOnDestruct = true);
    void setSuccess(bool success);
    void setXIsEven(bool xIsEven);
    ~SomeSlotFinishedSignalEmitterOnDestruct();
private:
    void setEmitOnDestruct(bool emitOnDestruct); //orig had this public, but it might be abused instead of deferResponding/assumeResponsibilityToRespond
    friend class SomeSlotRequestResponse;
    friend class SomeSlotScopedResponder;

    bool m_EmitOnDestruct;
    bool m_Success;
    bool m_XIsEven;
signals:
    void signalToBeEmittedInDestructor(bool success, bool xIsEven);
};

class SomeSlotScopedResponder : public std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>
{
public:
    SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent);
    void reset(SomeSlotFinishedSignalEmitterOnDestruct *data)=delete;
    void deferResponding();
private:
    void reset(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent);
    friend class SomeSlotRequestResponse;

    SomeSlotRequestResponse *m_Parent;
};

}

#endif // SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H
