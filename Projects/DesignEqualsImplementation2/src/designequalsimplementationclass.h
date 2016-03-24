#ifndef DESIGNEQUALSIMPLEMENTATIONCLASS_H
#define DESIGNEQUALSIMPLEMENTATIONCLASS_H

#include "designequalsimplementationtype.h"

#include <QPair>

#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassprivatemethod.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "designequalsimplementationclassmethodargument.h"

#define DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX "failed to generate source for: "

class DesignEqualsImplementationProject;

//typedef QPair<QString /*member variable name*/, DesignEqualsImplementationClass* /*member variable type*/> HasA_Private_Classes_Members_ListEntryType;

//TODOinstancing: class DesignEqualsImplementationClassInstance;

class DesignEqualsImplementationClass : public DesignEqualsImplementationType
{
    Q_OBJECT
public:
    //static DesignEqualsImplementationClassInstance *createClassInstance(DesignEqualsImplementationClassInstance *parent = 0, const QString &optionalVariableName = QString());
    static QString generateRawConnectStatementWithEndingSemicolon(const QString &signalObjectVariableName, const QString &signalNameIncludingNormalizedArgs, const QString &slotObjectVariableName, const QString &slotNameIncludingNormalizedArgs);

    explicit DesignEqualsImplementationClass(QObject *parent = 0, DesignEqualsImplementationProject *parentProject = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    virtual ~DesignEqualsImplementationClass();

    DesignEqualsImplementationClassSignal *createNewSignal(const QString &newSignalName = QString(), const QList<FunctionArgumentTypedef> &newSignalArgs = QList<FunctionArgumentTypedef>());
    void addSignal(DesignEqualsImplementationClassSignal *signalToAdd);

    DesignEqualsImplementationClassSlot *createwNewSlot(const QString &newSlotName = QString(), const QList<FunctionArgumentTypedef> &newSlotArgs = QList<FunctionArgumentTypedef>());
    void addSlot(DesignEqualsImplementationClassSlot *slotToAdd);
    void removeSlot(DesignEqualsImplementationClassSlot *slotToRemove);

    void addNonFunctionMember(NonFunctionMember *nonFunctionMember);

    DesignEqualsImplementationClassProperty *createNewProperty(DesignEqualsImplementationType *propertyType, const QString &qualifiedTypeString, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);

    QList<DesignEqualsImplementationClassProperty*> properties() const;
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSignal*> mySignals() const;
    QList<DesignEqualsImplementationClassSlot*> mySlots() const;

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QList<DesignEqualsImplementationClassSignal*> m_MySignals;
    QList<DesignEqualsImplementationClassSlot*> m_MySlots;

    inline int serializationPrivateMethodIdForPrivateMethod(DesignEqualsImplementationClassPrivateMethod *privateMethod) { return PrivateMethods.indexOf(privateMethod); }
    inline DesignEqualsImplementationClassPrivateMethod* privateMethodInstantatiationFromSerializedPrivateMethodId(int privateMethodId) { return PrivateMethods.at(privateMethodId); }
    inline int serializationSignalIdForSignal(DesignEqualsImplementationClassSignal *theSignal) { return m_MySignals.indexOf(theSignal); }
    inline DesignEqualsImplementationClassSignal* signalInstantiationFromSerializedSignalId(int signalId) { return m_MySignals.at(signalId); }
    inline int serializationSlotIdForSlot(DesignEqualsImplementationClassSlot *theSlot) { return m_MySlots.indexOf(theSlot); } //TODOoptional: check return of indexOf, qFatal. also on similar methods throughout
    inline DesignEqualsImplementationClassSlot *slotInstantiationFromSerializedSlotId(int slotId) { return m_MySlots.at(slotId); }

    QString nextTempUnnamedSlotName();

    inline bool slotWithThisNameExists(const QString &slotName)
    {
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, m_MySlots)
        {
            if(currentSlot->Name == slotName)
                return true;
        }
        return false;
    }
    int typeCategory() const { return 0; }
private:
    friend class DesignEqualsImplementationProjectGenerator;
signals:
    void nonFunctionMemberAdded(NonFunctionMember *nonFunctionMember);
    void privateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void slotAdded(DesignEqualsImplementationClassSlot*);
    void signalAdded(DesignEqualsImplementationClassSignal*);
};
#if 0
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator<<(QDataStream &out, DesignEqualsImplementationClass *designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *designEqualsImplementationClass);
#endif

Q_DECLARE_METATYPE(DesignEqualsImplementationClass*)

#if 0 //TODOinstancing
class DesignEqualsImplementationClassInstance : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit DesignEqualsImplementationClassInstance()
        : IHaveTypeAndVariableNameAndPreferredTextualRepresentation()
        , m_InstanceType(NoInstanceChosen)
        , m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline(0) { }
    explicit DesignEqualsImplementationClassInstance(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClass *parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, const QString &variableName)
        : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName)
        , m_MyClass(designEqualsImplementationClass)
        , m_InstanceType(parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline ? ChildMemberOfOtherClassLifeline : UseCasesRootClassLifeline)
        , m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline(parentClassThatHasMe_OrZeroIfUseCasesRootClassLifeline)
    {
        //m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.second = myIndexIntoParentHasAThatIsMe;
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.removeAll(this);
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.append(this);
        }
#endif
    }
    virtual ~DesignEqualsImplementationClassInstance()
    {
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline.first->m_ChildInstances.removeAll(this);
        }
#endif
    }
    virtual QString typeString()
    {
        return m_MyClass->Name + " *";
    }

    DesignEqualsImplementationClass *m_MyClass;
    //QPair<DesignEqualsImplementationClass*, int> m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfUseCasesRootClassLifeline;
    DesignEqualsImplementationClassInstanceTypeEnum m_InstanceType;
    DesignEqualsImplementationClass *m_ParentClassInstanceThatHasMe_OrZeroIfNoInstanceChosenOrUseCasesRootClassLifeline;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);

Q_DECLARE_METATYPE(DesignEqualsImplementationClassInstance*)
#endif

#endif // DESIGNEQUALSIMPLEMENTATIONCLASS_H
