#ifndef DESIGNEQUALSIMPLEMENTATIONCLASS_H
#define DESIGNEQUALSIMPLEMENTATIONCLASS_H

#include <QObject>
#include "idesignequalsimplementationvisuallyrepresenteditem.h"

#include <QPair>

#include "designequalsimplementationclassproperty.h"
#include "designequalsimplementationclassprivatemethod.h"
#include "designequalsimplementationclassslot.h"
#include "designequalsimplementationclasssignal.h"

#define DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX "failed to generate source for: "

//typedef QPair<QString /*member variable name*/, DesignEqualsImplementationClass* /*member variable type*/> HasA_Private_Classes_Members_ListEntryType;

class DesignEqualsImplementationClassInstance;
class HasA_Private_PODorNonDesignedCpp_Members_ListEntryType;

class DesignEqualsImplementationClass : public QObject, public IDesignEqualsImplementationVisuallyRepresentedItem
{
    Q_OBJECT
public:
    //static DesignEqualsImplementationClassInstance *createClassInstance(DesignEqualsImplementationClassInstance *parent = 0, const QString &optionalVariableName = QString());

    explicit DesignEqualsImplementationClass(QObject *parent = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    virtual ~DesignEqualsImplementationClass();

    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *createNewHasAPrivate_PODorNonDesignedCpp_Member(const QString &typeString = QString(), const QString &variableName = QString());

    DesignEqualsImplementationClassSignal *createNewSignal(const QString &newSignalName = QString());

    DesignEqualsImplementationClassSlot *createwNewSlot(const QString &newSlotName = QString());
    void addSlot(DesignEqualsImplementationClassSlot *slotToAdd);
    void removeSlot(DesignEqualsImplementationClassSlot *slotToRemove);

    //TODOoptional: private + getter/setter blah
    QString ClassName;
    QList<DesignEqualsImplementationClassProperty*> Properties;
    DesignEqualsImplementationClassInstance* createHasA_Private_Classes_Member(DesignEqualsImplementationClass *hasA_Private_Class_Member, const QString &variableName);
    QList<DesignEqualsImplementationClassInstance*> hasA_Private_Classes_Members();
    QList<HasA_Private_PODorNonDesignedCpp_Members_ListEntryType*> hasA_Private_PODorNonDesignedCpp_Members();
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSignal*> mySignals();
    QList<DesignEqualsImplementationClassSlot*> mySlots();

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QString headerFilenameOnly();


    //TODOoptional: should be private
    QList<DesignEqualsImplementationClassInstance*> m_HasA_Private_Classes_Members;
    QList<HasA_Private_PODorNonDesignedCpp_Members_ListEntryType*> m_HasA_Private_PODorNonDesignedCpp_Members;
    QList<DesignEqualsImplementationClassSignal*> m_MySignals;
    QList<DesignEqualsImplementationClassSlot*> m_MySlots;

    //Temporary for code gen
    //void appendLineToClassConstructorTemporarily(const QString &line);
private:
    //QList<QString> m_TemporaryClassConstructorLines;
signals:
    void propertyAdded(DesignEqualsImplementationClassProperty*);
    void hasAPrivateMemberClassAdded(DesignEqualsImplementationClassInstance); //declare meta type?
    void privateMethodAdded(DesignEqualsImplementationClassPrivateMethod*);
    void slotAdded(DesignEqualsImplementationClassSlot*);
    void signalAdded(DesignEqualsImplementationClassSignal*);
    void e(const QString &);
public slots:
    void emitAllClassDetails();
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass);

Q_DECLARE_METATYPE(DesignEqualsImplementationClass*)

class DesignEqualsImplementationClassInstance : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit DesignEqualsImplementationClassInstance() : IHaveTypeAndVariableNameAndPreferredTextualRepresentation() { }
    explicit DesignEqualsImplementationClassInstance(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClass *parentClassThatHasMe_OrZeroIfTopLevelObject, int myIndexIntoParentHasAThatIsMe, const QString &variableName) : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName), m_MyClass(designEqualsImplementationClass)
    {
        m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfTopLevelObject.first = parentClassThatHasMe_OrZeroIfTopLevelObject;
        m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfTopLevelObject.second = myIndexIntoParentHasAThatIsMe;
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrZeroIfTopLevelObject.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrZeroIfTopLevelObject.first->m_ChildInstances.removeAll(this);
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrZeroIfTopLevelObject.first->m_ChildInstances.append(this);
        }
#endif
    }
    virtual ~DesignEqualsImplementationClassInstance()
    {
        qDeleteAll(m_ChildInstances);
#if 0
        if(m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrZeroIfTopLevelObject.first)
        {
            m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrZeroIfTopLevelObject.first->m_ChildInstances.removeAll(this);
        }
#endif
    }
    virtual QString typeString()
    {
        return m_MyClass->ClassName + " *";
    }

    DesignEqualsImplementationClass *m_MyClass;
    QPair<DesignEqualsImplementationClass*, int> m_ParentClassInstanceThatHasMe_AndMyIndexIntoHisHasAThatIsMe_OrFirstIsZeroIfTopLevelObject;
    QList<DesignEqualsImplementationClassInstance*> m_ChildInstances;
};
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType);

Q_DECLARE_METATYPE(DesignEqualsImplementationClassInstance*)

class HasA_Private_PODorNonDesignedCpp_Members_ListEntryType : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit HasA_Private_PODorNonDesignedCpp_Members_ListEntryType() : IHaveTypeAndVariableNameAndPreferredTextualRepresentation() { }
    explicit HasA_Private_PODorNonDesignedCpp_Members_ListEntryType(const QString &type, /*DesignEqualsImplementationClass *designEqualsImplementationClassThatHasMe,*/ const QString &variableName) : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName), Type(type) { }
    virtual ~HasA_Private_PODorNonDesignedCpp_Members_ListEntryType() { }
    virtual QString typeString()
    {
        return Type;
    }
    QString Type;
    //DesignEqualsImplementationClass *m_DesignEqualsImplementationClassThatHasMe;
};
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType);
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType);

#endif // DESIGNEQUALSIMPLEMENTATIONCLASS_H
