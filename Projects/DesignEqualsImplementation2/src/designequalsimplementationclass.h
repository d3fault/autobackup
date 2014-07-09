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

class HasA_Private_Classes_Members_ListEntryType;
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
    QList<HasA_Private_Classes_Members_ListEntryType*> HasA_Private_Classes_Members;
    QList<HasA_Private_PODorNonDesignedCpp_Members_ListEntryType*> HasA_Private_PODorNonDesignedCpp_Members;
    QList<DesignEqualsImplementationClassPrivateMethod*> PrivateMethods;
    QList<DesignEqualsImplementationClassSlot*> Slots;
    QList<DesignEqualsImplementationClassSignal*> Signals;

    //QList<QString> allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString();

    QString headerFilenameOnly();
signals:
    void propertyAdded(DesignEqualsImplementationClassProperty*);
    void hasAPrivateMemberClassAdded(HasA_Private_Classes_Members_ListEntryType); //declare meta type?
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

class HasA_Private_Classes_Members_ListEntryType : public IHaveTypeAndVariableNameAndPreferredTextualRepresentation
{
public:
    explicit HasA_Private_Classes_Members_ListEntryType() : IHaveTypeAndVariableNameAndPreferredTextualRepresentation() { }
    explicit HasA_Private_Classes_Members_ListEntryType(DesignEqualsImplementationClass *designEqualsImplementationClass, DesignEqualsImplementationClass *designEqualsImplementationClassThatHasMe, const QString &variableName) : IHaveTypeAndVariableNameAndPreferredTextualRepresentation(variableName), m_DesignEqualsImplementationClass(designEqualsImplementationClass), m_DesignEqualsImplementationClassThatHasMe(designEqualsImplementationClassThatHasMe) { }
    virtual ~HasA_Private_Classes_Members_ListEntryType() { }
    virtual QString typeString()
    {
        return m_DesignEqualsImplementationClass->ClassName + " *";
    }

    DesignEqualsImplementationClass *m_DesignEqualsImplementationClass;
    DesignEqualsImplementationClass *m_DesignEqualsImplementationClassThatHasMe;
};
QDataStream &operator<<(QDataStream &out, const HasA_Private_Classes_Members_ListEntryType &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Members_ListEntryType &hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator<<(QDataStream &out, const HasA_Private_Classes_Members_ListEntryType *&hasA_Private_Classes_Members_ListEntryType);
QDataStream &operator>>(QDataStream &in, HasA_Private_Classes_Members_ListEntryType *&hasA_Private_Classes_Members_ListEntryType);

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
