#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include <QMutexLocker>
#include "designequalsimplementation.h"

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationsignalemissionstatement.h"

#define DesignEqualsImplementationClass_QDS(qds, direction, designEqualsImplementationClass) \
qds direction designEqualsImplementationClass.Properties; \
qds direction designEqualsImplementationClass.m_HasA_Private_Classes_Members; \
qds direction designEqualsImplementationClass.HasA_Private_PODorNonDesignedCpp_Members; \
qds direction designEqualsImplementationClass.PrivateMethods; \
qds direction designEqualsImplementationClass.Slots; \
qds direction designEqualsImplementationClass.Signals; \
return qds;

#define HasA_Private_Classes_Members_ListEntryType_QDS(qds, direction, hasA_Private_Classes_Members_ListEntryType) \
qds direction hasA_Private_Classes_Members_ListEntryType.VariableName; \
qds direction *hasA_Private_Classes_Members_ListEntryType.m_MyClass; \
return qds;

#define HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(qds, direction, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType) \
qds direction hasA_Private_PODorNonDesignedCpp_Members_ListEntryType.VariableName; \
qds direction hasA_Private_PODorNonDesignedCpp_Members_ListEntryType.Type; \
return qds;

//TODOoptional: auto-pimpl, since pimpl is cheap/awesome (and gives us implicit sharing when done properly) and increases source/binary compatibility. MAYBE it should be opt-in, but probably opt-out instead?
#if 0
DesignEqualsImplementationClassInstance *DesignEqualsImplementationClass::createClassInstance(DesignEqualsImplementationClassInstance *parent, const QString &optionalVariableName) //top-level objects don't need a variable name if there's only one use case being generated in lib mode, for example
{
    return new DesignEqualsImplementationClassInstance(this, parent, optionalVariableName);
}
#endif
DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
    , IDesignEqualsImplementationVisuallyRepresentedItem()
{ }
bool DesignEqualsImplementationClass::generateSourceCode(const QString &destinationDirectoryPath)
{
    //Header
    QString headerFilePath = destinationDirectoryPath + headerFilenameOnly();
    QFile headerFile(headerFilePath);
    if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open header for writing: " + headerFilePath);
        return false;
    }
    QTextStream headerFileTextStream(&headerFile);

    //Source
    QString sourceFilePath = destinationDirectoryPath + ClassName.toLower() + ".cpp";
    QFile sourceFile(sourceFilePath);
    if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open source for writing: " + sourceFilePath);
        return false;
    }
    QTextStream sourceFileTextStream(&sourceFile);

#if 0 //endl vs. "\n"
    //PrivateMembers
    //comes before both source/header headers because needs to append/insert forward declares (header) and includes (source)
    QString headerForwardDeclares; //thought about using a QList<QString> and doing the formatting in "Header's header", but it really makes no difference so KISS
    Q_FOREACH(PrivateMemberType currentPrivateMember, PrivateMembers)
    {
        //Forward declares
        headerForwardDeclares.append("class " + )
    }
#endif

    //Header's header (wat) + constructor
    QString myNameHeaderGuard = ClassName.toUpper() + "_H";
    headerFileTextStream    << "#ifndef " << myNameHeaderGuard << endl
                            << "#define " << myNameHeaderGuard << endl
                            << endl
                            << "#include <QObject>" << endl //TODOoptional: non-QObject classes? hmm nah because signals/slots based
                            << endl;
    //Header's header's forward declares
    bool atLeastOneHasAPrivateMemberClass = !hasA_Private_Classes_Members().isEmpty(); //spacing
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentPrivateMember, hasA_Private_Classes_Members())
    {
        //class Bar;
        headerFileTextStream << "class " << currentPrivateMember->m_MyClass->ClassName << ";" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        headerFileTextStream << endl; //OCD <3
    headerFileTextStream    << "class " << ClassName << " : public QObject" << endl
                            << "{" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl
                            << "public:" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << ClassName << "(QObject *parent = 0);" << endl;
                            //<< DESIGNEQUALSIMPLEMENTATION_TAB << "~" << Name << "();" << endl;
    //Header's hasAPrivateMemberClass declarations
    if(atLeastOneHasAPrivateMemberClass)
        headerFileTextStream << "private:" << endl;
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentPrivateMember, hasA_Private_Classes_Members())
    {
        //Bar *m_Bar;
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentPrivateMember->preferredTextualRepresentation() << ";" << endl;
    }

    //Source's header+constructor (the top bits, not the ".h" counter-part)
    sourceFileTextStream    << "#include \"" << headerFilenameOnly() << "\"" << endl
                            << endl;
    //Source's header PrivateMemberClasses includes
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentPrivateMember, hasA_Private_Classes_Members())
    {
        //#include "bar.h"
        sourceFileTextStream << "#include \"" << currentPrivateMember->m_MyClass->headerFilenameOnly() << "\"" << endl;
    }
    if(atLeastOneHasAPrivateMemberClass)
        sourceFileTextStream << endl;
    sourceFileTextStream    << ClassName << "::" << ClassName << "(QObject *parent)" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl;
    //Source's header PrivateMemberClasses constructor initializers
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentPrivateMember, hasA_Private_Classes_Members())
    {
        //, m_Bar(new Bar(this))
        sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ", " << currentPrivateMember->VariableName << "(new " << currentPrivateMember->m_MyClass->ClassName << "(this))" << endl; //TODOreq: for now all my objects need a QObject *parent=0 constructor, but since that's also a [fixable] requirement for my ObjectOnThreadGroup, no biggy. Still, would be nice to solve the threading issue and to allow constructor args here (RAII = pro)
    }
    sourceFileTextStream    << "{ }" << endl;

    bool signalsAccessSpecifierWritten = false;
    //Signals
    if(!Signals.isEmpty())
    {
        headerFileTextStream << "signals:" << endl;
        signalsAccessSpecifierWritten = true;
    }
    Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, Signals)
    {
        //void fooSignal();
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSignal->methodSignatureWithoutReturnType() << ";" << endl;
    }

    //Slots
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, Slots)
    {
        //slot finished/exit signal
        //each slot/unit-of-execution/ has the opportunity of supplying (by connecting to actor) a finished/exit signal
        if(currentSlot->finishedOrExitSignal())
        {
            if(!signalsAccessSpecifierWritten)
            {
                headerFileTextStream << "signals:" << endl;
                signalsAccessSpecifierWritten = true;
            }
            headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSlot->finishedOrExitSignal()->methodSignatureWithoutReturnType() << ";" << endl;
        }
    }
    if(!Slots.isEmpty())
        headerFileTextStream << "public slots:" << endl;
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, Slots)
    {
        //Declare slot
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSlot->methodSignatureWithoutReturnType() << ";" << endl;
        //Define slot
        sourceFileTextStream    << "void " << ClassName << "::" << currentSlot->methodSignatureWithoutReturnType() << endl
                                << "{" << endl;
        Q_FOREACH(IDesignEqualsImplementationStatement *currentSlotCurrentStatement, currentSlot->orderedListOfStatements())
        {
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentSlotCurrentStatement->toRawCppWithEndingSemicolon() << endl;
        }
        if(currentSlot->finishedOrExitSignal())
        {
            DesignEqualsImplementationSignalEmissionStatement finishedOrExitSignalEmitStatement(currentSlot->finishedOrExitSignal(), currentSlot->finishedOrExitSignalEmissionContextVariables());
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << finishedOrExitSignalEmitStatement.toRawCppWithEndingSemicolon() << endl;
        }
        sourceFileTextStream    << "}" << endl;
    }

    //Header's footer
    headerFileTextStream    << "};" << endl
                            << endl
                            << "#endif // " << myNameHeaderGuard << endl;

    //Recursively generate source for all children HasA_PrivateMemberClasses
    Q_FOREACH(DesignEqualsImplementationClassInstance *currentPrivateMember, hasA_Private_Classes_Members())
    {
        if(!currentPrivateMember->m_MyClass->generateSourceCode(destinationDirectoryPath))
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX + currentPrivateMember->m_MyClass->ClassName);
            return false;
        }
    }

    return true;
}
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    //Q_FOREACH(HasA_Private_Classes_Members_ListEntryType *currentMember, HasA_PrivateMemberClasses)
    //{
    //    delete currentMember->m_DesignEqualsImplementationClass;
    //}
    qDeleteAll(m_HasA_Private_Classes_Members);
    qDeleteAll(HasA_Private_PODorNonDesignedCpp_Members);
    qDeleteAll(PrivateMethods);
    qDeleteAll(Slots);
    qDeleteAll(Signals);
}
DesignEqualsImplementationClassInstance* DesignEqualsImplementationClass::addHasA_Private_Classes_Member(DesignEqualsImplementationClass *hasA_Private_Class_Member, const QString &variableName)
{
    //TODOreq: ensure all callers haven't already done the "new"

    int newInex = m_HasA_Private_Classes_Members.size();
    DesignEqualsImplementationClassInstance *newInstance = new DesignEqualsImplementationClassInstance(hasA_Private_Class_Member, this, newInex, variableName);
    m_HasA_Private_Classes_Members.append(newInstance); //TODOreq: re-ordering needs to resynchronize

    return newInstance;
}
QList<DesignEqualsImplementationClassInstance *> DesignEqualsImplementationClass::hasA_Private_Classes_Members()
{
    return m_HasA_Private_Classes_Members;
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
QString DesignEqualsImplementationClass::headerFilenameOnly()
{
    return ClassName.toLower() + ".h";
}
void DesignEqualsImplementationClass::emitAllClassDetails()
{
    //QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(out, <<, designEqualsImplementationClass);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass &designEqualsImplementationClass)
{
    DesignEqualsImplementationClass_QDS(in, >>, designEqualsImplementationClass);
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    return out << *designEqualsImplementationClass;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClass *&designEqualsImplementationClass)
{
    designEqualsImplementationClass = new DesignEqualsImplementationClass();
    return in >> *designEqualsImplementationClass;
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType)
{
    HasA_Private_Classes_Members_ListEntryType_QDS(out, <<, hasA_Private_Classes_Members_ListEntryType)
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance &hasA_Private_Classes_Members_ListEntryType)
{
    HasA_Private_Classes_Members_ListEntryType_QDS(in, >>, hasA_Private_Classes_Members_ListEntryType)
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType)
{
    return out << *hasA_Private_Classes_Members_ListEntryType;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationClassInstance *&hasA_Private_Classes_Members_ListEntryType)
{
    hasA_Private_Classes_Members_ListEntryType = new DesignEqualsImplementationClassInstance();
    return in >> *hasA_Private_Classes_Members_ListEntryType;
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(out, <<, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
}
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType &hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    HasA_Private_PODorNonDesignedCpp_Members_ListEntryType_QDS(in, >>, hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
}
QDataStream &operator<<(QDataStream &out, const HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    return out << *hasA_Private_PODorNonDesignedCpp_Members_ListEntryType;
}
QDataStream &operator>>(QDataStream &in, HasA_Private_PODorNonDesignedCpp_Members_ListEntryType *&hasA_Private_PODorNonDesignedCpp_Members_ListEntryType)
{
    hasA_Private_PODorNonDesignedCpp_Members_ListEntryType = new HasA_Private_PODorNonDesignedCpp_Members_ListEntryType();
    return in >> *hasA_Private_PODorNonDesignedCpp_Members_ListEntryType;
}
