#ifndef DESIGNEQUALSIMPLEMENTATIONSIGNALSLOTINTERFACE_H
#define DESIGNEQUALSIMPLEMENTATIONSIGNALSLOTINTERFACE_H

#if 0

#include <QObject>

class DesignEqualsImplementationClassSlot;

class DesignEqualsImplementationSignalSlotInterface : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationSignalSlotInterface(QObject *parent = 0);
private:
    QList<QPair<QString /*signalName*/, QString /*normalizedSignalSignature (preferably keep arg names too)*/> > m_SignalsInThisInterface;
    QString interfaceEntryPoint;
};
#endif

#endif // DESIGNEQUALSIMPLEMENTATIONSIGNALSLOTINTERFACE_H
