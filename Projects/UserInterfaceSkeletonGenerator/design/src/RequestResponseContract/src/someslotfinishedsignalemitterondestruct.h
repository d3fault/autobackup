#ifndef SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H
#define SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H

#include <QObject>

#include <memory>

class SomeSlotRequestResponse;

class SomeSlotFinishedSignalEmitterOnDestruct : public QObject
{
    Q_OBJECT
public:
    explicit SomeSlotFinishedSignalEmitterOnDestruct();
    void setSuccess(bool success);
    ~SomeSlotFinishedSignalEmitterOnDestruct();
private:    
    bool m_Success;
signals:
    void signalToBeEmittedInDestructor(bool success);
};

class SomeSlotScopedResponder : public std::unique_ptr<SomeSlotFinishedSignalEmitterOnDestruct>
{
public:
    SomeSlotScopedResponder(SomeSlotFinishedSignalEmitterOnDestruct *data, SomeSlotRequestResponse *parent);
    void deferResponding();
private:
    SomeSlotRequestResponse *m_Parent;
};

#endif // SOMESLOTFINISHEDSIGNALEMITTERONDESTRUCT_H
