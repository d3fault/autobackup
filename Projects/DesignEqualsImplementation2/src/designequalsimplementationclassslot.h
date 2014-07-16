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
    void setFinishedOrExitSignal(DesignEqualsImplementationClassSignal *finishedOrExitSignal, SignalEmissionOrSlotInvocationContextVariables finishedOrExitSignalEmissionContextVariables);
    DesignEqualsImplementationClassSignal *finishedOrExitSignal() const;
    SignalEmissionOrSlotInvocationContextVariables finishedOrExitSignalEmissionContextVariables() const;
    //TODOoptional: return type

    virtual QObject *asQObject();
private:
    //DesignEqualsImplementationClassLifeLine *m_ParentClassLifeLineInUseCaseView_OrZeroInClassDiagramView;
    DesignEqualsImplementationClassSignal *m_FinishedOrExitSignal;
    SignalEmissionOrSlotInvocationContextVariables m_ExitSignalEmissionContextVariables;
signals:
    void statementInserted(int indexInsertedInto, IDesignEqualsImplementationStatement *statementInserted);
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot &slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot &slot);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassSlot *&slot);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassSlot *&slot);

Q_DECLARE_METATYPE(DesignEqualsImplementationClassSlot*)

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSSLOT_H
