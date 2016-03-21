#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationprojectgenerator.h" //only for a few string utils (/lazy)

#if 0
DesignEqualsImplementationClassInstance *DesignEqualsImplementationClass::createClassInstance(DesignEqualsImplementationClassInstance *parent, const QString &optionalVariableName) //top-level objects don't need a variable name if there's only one use case being generated in lib mode, for example
{
    return new DesignEqualsImplementationClassInstance(this, parent, optionalVariableName);
}
#endif
QString DesignEqualsImplementationClass::generateRawConnectStatementWithEndingSemicolon(const QString &signalObjectVariableName, const QString &signalNameIncludingNormalizedArgs, const QString &slotObjectVariableName, const QString &slotNameIncludingNormalizedArgs)
{
    return QString("connect(" + signalObjectVariableName + ", SIGNAL(" + signalNameIncludingNormalizedArgs + "), " +  slotObjectVariableName + ", SLOT(" + slotNameIncludingNormalizedArgs + "));");
}
DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent, DesignEqualsImplementationProject *parentProject)
    : Type(parent)
    , IDesignEqualsImplementationVisuallyRepresentedItem()
    , m_ParentProject(parentProject)
{ }
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(PrivateMethods);
    qDeleteAll(m_MySlots);
    qDeleteAll(m_MySignals);
}
DesignEqualsImplementationClassSignal *DesignEqualsImplementationClass::createNewSignal(const QString &newSignalName, const QList<FunctionArgumentTypedef> &newSignalArgs)
{
    DesignEqualsImplementationClassSignal *newSignal = new DesignEqualsImplementationClassSignal(this);
    newSignal->Name = newSignalName;
    Q_FOREACH(const FunctionArgumentTypedef &newSignalArg, newSignalArgs)
    {
        newSignal->createNewArgument(m_ParentProject->getOrCreateTypeFromName(newSignalArg.NonQualifiedType), newSignalArg.QualifiedType, newSignalArg.Name);
    }
    addSignal(newSignal);
    return newSignal;
}
void DesignEqualsImplementationClass::addSignal(DesignEqualsImplementationClassSignal *signalToAdd)
{
    signalToAdd->ParentClass = this;
    m_MySignals.append(signalToAdd);
    emit signalAdded(signalToAdd);
}
DesignEqualsImplementationClassSlot *DesignEqualsImplementationClass::createwNewSlot(const QString &newSlotName, const QList<FunctionArgumentTypedef> &newSlotArgs)
{
    DesignEqualsImplementationClassSlot *newSlot = new DesignEqualsImplementationClassSlot(this);
    newSlot->Name = newSlotName;
    Q_FOREACH(const FunctionArgumentTypedef &newSlotArg, newSlotArgs)
    {
        newSlot->createNewArgument(m_ParentProject->getOrCreateTypeFromName(newSlotArg.NonQualifiedType), newSlotArg.QualifiedType, newSlotArg.Name);
    }
    addSlot(newSlot);
    return newSlot;
}
void DesignEqualsImplementationClass::addSlot(DesignEqualsImplementationClassSlot *slotToAdd)
{
    //connect(slotToAdd, SIGNAL(e(QString)))
    m_MySlots.append(slotToAdd);
    slotToAdd->ParentClass = this;
    emit slotAdded(slotToAdd);
}
void DesignEqualsImplementationClass::removeSlot(DesignEqualsImplementationClassSlot *slotToRemove)
{
    m_MySlots.removeOne(slotToRemove);
    slotToRemove->ParentClass = 0; //TODOreq: a slot without a parent is undefined
    //emit slotRemoved(slotToRemove);
}
void DesignEqualsImplementationClass::addNonFunctionMember(NonFunctionMember *nonFunctionMember)
{
    //DesignEqualsImplementationClass accepts any kind of NonFunctionMember
    addNonFunctionMemberPrivate(nonFunctionMember);
}
DesignEqualsImplementationClassProperty *DesignEqualsImplementationClass::createNewProperty(Type *propertyType, const QString &qualifiedTypeString, const QString &propertyName, bool hasInit, const QString &optionalInit, bool readOnly, bool notifiesOnChange)
{
    DesignEqualsImplementationClassProperty *newProperty = new DesignEqualsImplementationClassProperty(propertyType, qualifiedTypeString, propertyName, this, this, hasInit, optionalInit, readOnly, notifiesOnChange);
    addNonFunctionMember(newProperty);
    return newProperty;
}
QList<DesignEqualsImplementationClassProperty*> DesignEqualsImplementationClass::properties() const
{
    //TODOoptimization: if callers call this multiple times in a row, they should re-use the first call. a different way to optimize yet is for the callers to do the qobject_cast'ing, but not all uses would warrant that (but if we're already iterating nonfunctionmembers, for example)
    QList<DesignEqualsImplementationClassProperty*> ret;
    Q_FOREACH(NonFunctionMember *currentNonFunctionMember, nonFunctionMembers())
    {
        if(DesignEqualsImplementationClassProperty *memberAsPropertyMaybe = qobject_cast<DesignEqualsImplementationClassProperty*>(currentNonFunctionMember))
        {
            ret << memberAsPropertyMaybe;
        }
    }
    return ret;
}
QList<DesignEqualsImplementationClassSignal *> DesignEqualsImplementationClass::mySignals()
{
    return m_MySignals;
}
QList<DesignEqualsImplementationClassSlot*> DesignEqualsImplementationClass::mySlots()
{
    return m_MySlots;
}
//Hmm now that I come to actually using this, why lose the pointers and resort to strings :)? Only thing though is that I need to refactor so that hasAClasses, properties, and localVariables(undefined-atm) all derive from some shared base "variable" xD. And shit local variables won't be introduced until C++ drop down mode is implemented (or at least designed), and even then they still might not ever show up in a slot-unit-of-execution-thingo.
#if 0
QList<QString> DesignEqualsImplementationClass::allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString()
{
    //Properties and PrivateMembers
    QList<QString> ret;

    //TODOreq: properties. i'm not too familiar with Q_PROPERTY, even though obviously i want to support it. i'm not sure, but i think i want the "read" part of the property here...

    Q_FOREACH(HasA_Private_Classes_Members_ListEntryType currentMember, HasA_PrivateMemberClasses)
    {
        ret.append(currentMember.VariableName);
    }
    return ret;
}
#endif
QString DesignEqualsImplementationClass::nextTempUnnamedSlotName()
{
    QString ret;
    int i = -1;
    do
    {
        ret = UseCaseGraphicsScene_TEMP_SLOT_MAGICAL_NAME_STRING_PREFIX + QString::number(++i);
    }
    while(slotWithThisNameExists(ret));
    return ret;
}
void DesignEqualsImplementationClass::setClassName(const QString &newClassName)
{
    Name = newClassName;
}
void DesignEqualsImplementationClass::emitAllClassDetails()
{
    //TODOreq
}
