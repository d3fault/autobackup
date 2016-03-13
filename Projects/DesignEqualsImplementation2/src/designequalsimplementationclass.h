#ifndef DESIGNEQUALSIMPLEMENTATIONCLASS_H
#define DESIGNEQUALSIMPLEMENTATIONCLASS_H

#include "type.h"
#include "idesignequalsimplementationvisuallyrepresenteditem.h"

#include <QPair>

#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassprivatemethod.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"
#include "hasa_private_classes_member.h"
#include "designequalsimplementationclassmethodargument.h"

#define DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX "failed to generate source for: "

class DesignEqualsImplementationProject;

//typedef QPair<QString /*member variable name*/, DesignEqualsImplementationClass* /*member variable type*/> HasA_Private_Classes_Members_ListEntryType;

//TODOinstancing: class DesignEqualsImplementationClassInstance;

class DesignEqualsImplementationClass : public Type, public IDesignEqualsImplementationVisuallyRepresentedItem
{
    Q_OBJECT
public:
    //static DesignEqualsImplementationClassInstance *createClassInstance(DesignEqualsImplementationClassInstance *parent = 0, const QString &optionalVariableName = QString());
    static QString generateRawConnectStatementWithEndingSemicolon(const QString &signalObjectVariableName, const QString &signalNameIncludingNormalizedArgs, const QString &slotObjectVariableName, const QString &slotNameIncludingNormalizedArgs);

    explicit DesignEqualsImplementationClass(QObject *parent = 0, DesignEqualsImplementationProject *parentProject = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    virtual ~DesignEqualsImplementationClass();

    DesignEqualsImplementationClassSignal *createNewSignal(const QString &newSignalName = QString(), const QList<MethodArgumentTypedef> &newSignalArgs = QList<MethodArgumentTypedef>());
    void addSignal(DesignEqualsImplementationClassSignal *signalToAdd);

    DesignEqualsImplementationClassSlot *createwNewSlot(const QString &newSlotName = QString(), const QList<MethodArgumentTypedef> &newSlotArgs = QList<MethodArgumentTypedef>());
    void addSlot(DesignEqualsImplementationClassSlot *slotToAdd);
    void removeSlot(DesignEqualsImplementationClassSlot *slotToRemove);

    //TODOoptional: private + getter/setter blah
    DesignEqualsImplementationProject *m_ParentProject;
    QString ClassName;

    bool addNonFunctionMember(NonFunctionMember *nonFunctionMember);

    DesignEqualsImplementationClassProperty *createNewProperty(Type *propertyType, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange);
    void addProperty(DesignEqualsImplementationClassProperty *propertyToAdd);

    //HasA_Private_Classes_Member *createHasA_Private_Classes_Member(DesignEqualsImplementationClass *memberClassType, const QString &variableName_OrLeaveBlankForAutoNumberedVariableName = QString());

    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSignal*> mySignals();
    QList<DesignEqualsImplementationClassSlot*> mySlots();

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QString headerFilenameOnly();
    QString sourceFilenameOnly();


    //TODOoptional: should be private
    QList<DesignEqualsImplementationClassProperty*> Properties;
    QList<DesignEqualsImplementationClassSignal*> m_MySignals;
    QList<DesignEqualsImplementationClassSlot*> m_MySlots;

    inline int serializationPrivateMethodIdForPrivateMethod(DesignEqualsImplementationClassPrivateMethod *privateMethod) { return PrivateMethods.indexOf(privateMethod); }
    inline DesignEqualsImplementationClassPrivateMethod* privateMethodInstantatiationFromSerializedPrivateMethodId(int privateMethodId) { return PrivateMethods.at(privateMethodId); }
    inline int serializationSignalIdForSignal(DesignEqualsImplementationClassSignal *theSignal) { return m_MySignals.indexOf(theSignal); }
    inline DesignEqualsImplementationClassSignal* signalInstantiationFromSerializedSignalId(int signalId) { return m_MySignals.at(signalId); }
    inline int serializationSlotIdForSlot(DesignEqualsImplementationClassSlot *theSlot) { return m_MySlots.indexOf(theSlot); } //TODOoptional: check return of indexOf, qFatal. also on similar methods throughout
    inline DesignEqualsImplementationClassSlot *slotInstantiationFromSerializedSlotId(int slotId) { return m_MySlots.at(slotId); }
    inline int serializationHasAIdForNonFunctionMember(NonFunctionMember *theNonFunctionMember) { return m_NonFunctionMembers.indexOf(theNonFunctionMember); }
    inline NonFunctionMember *nonFunctionMemberFromNonFunctionMemberId(int nonFunctionMemberId) { return m_NonFunctionMembers.at(nonFunctionMemberId); }

    QString autoNameForNewChildMemberOfType(DesignEqualsImplementationClass *childMemberClassType);
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
private:
    friend class DesignEqualsImplementationProjectGenerator;
signals:
    void propertyAdded(DesignEqualsImplementationClassProperty*);
    void hasAPrivateMemberClassAdded(HasA_Private_Classes_Member*); //declare meta type?
    void privateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void slotAdded(DesignEqualsImplementationClassSlot*);
    void signalAdded(DesignEqualsImplementationClassSignal*);
    void e(const QString &);
public slots:
    void setClassName(const QString &newClassName);
    void emitAllClassDetails();
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
        return m_MyClass->ClassName + " *";
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
