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
    explicit DesignEqualsImplementationClass(QObject *parent = 0);
    bool generateSourceCode(const QString &destinationDirectoryPath);
    virtual ~DesignEqualsImplementationClass();

    //TODOoptional: private + getter/setter blah
    QString ClassName;
    QList<DesignEqualsImplementationClassProperty*> Properties;
    QList<DesignEqualsImplementationClassInstance*> HasA_Private_Classes_Members;
    QList<HasA_Private_PODorNonDesignedCpp_Members_ListEntryType*> HasA_Private_PODorNonDesignedCpp_Members;
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSlot*> Slots;
    QList<DesignEqualsImplementationClassSignal*> Signals;

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QString headerFilenameOnly();
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
    explicit DesignEqualsImplementationClassInstance(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClass *parentClassThatHasMe, const QString &variableName) : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName), m_MyClass(designEqualsImplementationClass), m_ParentClassThatHasMe_OrZeroIfTopLevelObject(parentClassThatHasMe) { }
    virtual ~DesignEqualsImplementationClassInstance() { }
    virtual QString typeString()
    {
        return m_MyClass->ClassName + " *";
    }

    DesignEqualsImplementationClass *m_MyClass;
    DesignEqualsImplementationClass *m_ParentClassThatHasMe_OrZeroIfTopLevelObject;
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
