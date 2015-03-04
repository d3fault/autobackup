#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H

#include "idesignequalsimplementationhaveorderedlistofstatements.h"
#include "idesignequalsimplementationmethod.h"

#include <QObject>
#include <QList>

#include "signalemissionorslotinvocationcontextvariables.h"

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassSignal;
class DesignEqualsImplementationClassLifeLine;

typedef QPair<int /*id of class in project*/, int /*id of slot in class*/> SerializableSlotIdType;

class DesignEqualsImplementationClassSlot : public QObject, public IDesignEqualsImplementationHaveOrderedListOfStatements, public IDesignEqualsImplementationMethod
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassSlot(QObject *parent = 0);
    explicit DesignEqualsImplementationClassSlot(DesignEqualsImplementationClassLifeLine *parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject, QObject *parent /*parent explicit only to remove ambiguity*/);
    virtual ~DesignEqualsImplementationClassSlot();

    //TODOoptional: private + getter/setter blah
    //void setParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject(DesignEqualsImplementationClassLifeLine *parentClassLifeLine);
    //DesignEqualsImplementationClassLifeLine* parentClassLifeLineInUseCaseView_OrZeroInClassDiagramView_OrZeroWhenFirstTimeSlotIsUsedInAnyUseCaseInTheProject() const;
    void setFinishedOrExitSignal(DesignEqualsImplementationClassSignal *finishedOrExitSignal_OrZeroIfNone, SignalEmissionOrSlotInvocationContextVariables finishedOrExitSignalEmissionContextVariables);
    DesignEqualsImplementationClassSignal *finishedOrExitSignal_OrZeroIfNone() const;
    SignalEmissionOrSlotInvocationContextVariables finishedOrExitSignalEmissionContextVariables() const;
    //TODOoptional: return type

    virtual QObject *asQObject();

    static DesignEqualsImplementationClassSlot* streamInSlotReference(DesignEqualsImplementationProject *project, QDataStream &in);
    static void streamOutSlotReference(DesignEqualsImplementationProject *project, DesignEqualsImplementationClassSlot *slot, QDataStream &out);
private:
    //DesignEqualsImplementationClassLifeLine *m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView;
    DesignEqualsImplementationClassLifeLine *m_ClassLifelineFirstDefinedIn_OrZeroIfNotDefinedInAnyClassLifelinesYet; //TODOreq: update when relevant. also [de-]serialize
    DesignEqualsImplementationClassSignal *m_FinishedOrExitSignal;
    SignalEmissionOrSlotInvocationContextVariables m_ExitSignalEmissionContextVariables;
signals:
    void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statementInserted);
};
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSlot &slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClassSlot *slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *slot);
#endif

Q_DECLARE_METATYPE(DesignEqualsImplementationClassSlot*)

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
