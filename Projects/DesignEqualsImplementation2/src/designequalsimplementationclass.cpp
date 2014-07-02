#include "designequalsimplementationclass.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include "designequalsimplementationcommon.h"

#define DesignEqualsImplementationClass_QDS(qds, direction, designEqualsImplementationClass) \
qds direction designEqualsImplementationClass.Properties; \
qds direction designEqualsImplementationClass.PrivateMembers; \
qds direction designEqualsImplementationClass.PrivateMethods; \
qds direction designEqualsImplementationClass.Slots; \
qds direction designEqualsImplementationClass.Signals; \
return qds;

DesignEqualsImplementationClass::DesignEqualsImplementationClass(QObject *parent)
    : QObject(parent)
{ }
bool DesignEqualsImplementationClass::generateSourceCode(const QString &destinationDirectoryPath)
{
    const QString &myNameToLower = ClassName.toLower();
    //Header
    QString headerFilenameOnly = myNameToLower + ".h";
    QString headerFilePath = destinationDirectoryPath + headerFilenameOnly;
    QFile headerFile(headerFilePath);
    if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open header for writing: " + headerFilePath);
        return false;
    }
    QTextStream headerFileTextStream(&headerFile);
    //Header's header (wat) + constructor
    QString myNameHeaderGuard = ClassName.toUpper() + "_H";
    headerFileTextStream    << "#ifndef " << myNameHeaderGuard << endl
                            << "#define " << myNameHeaderGuard << endl
                            << endl
                            << "#include <QObject>" << endl //TODOoptional: non-QObject classes? hmm nah because signals/slots based
                            << endl
                            << "class " << ClassName << " : public QObject" << endl
                            << "{" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl
                            << "public:" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << ClassName << "(QObject *parent = 0);" << endl;
                            //<< DESIGNEQUALSIMPLEMENTATION_TAB << "~" << Name << "();" << endl;

    //Source
    QString sourceFilePath = destinationDirectoryPath + myNameToLower + ".cpp";
    QFile sourceFile(sourceFilePath);
    if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open source for writing: " + sourceFilePath);
        return false;
    }
    QTextStream sourceFileTextStream(&sourceFile);
    //Source's header+constructor (the top bits, not the ".h" counter-part)
    sourceFileTextStream    << "#include \"" << headerFilenameOnly << "\"" << endl
                            << endl
                            << ClassName << "::" << ClassName << "(QObject *parent)" << endl
                            << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl
                            << "{ }" << endl;


    //Slots
    bool firstSlot = true;
    Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, Slots)
    {
        if(firstSlot)
        {
            headerFileTextStream << "public slots:" << endl;
            firstSlot = false;
        }
        QString currentSlotArgString;
        bool firstSlotArg = true;
        Q_FOREACH(DesignEqualsImplementationClassMethodArgument *currentSlotCurrentArgument, currentSlot->Arguments)
        {
            if(!firstSlotArg)
                currentSlotArgString.append(", ");
            currentSlotArgString.append(currentSlotCurrentArgument->Type + " " + currentSlotCurrentArgument->Name); //TODOoptional: if argType ends with *, &, etc, then don't put that space in between type and name (just visual shit, but i r perfectionist)
            firstSlotArg = false;
        }
        QString currentSlotSignatureWithoutReturnTypeOrEndingSemicolon = currentSlot->Name + "(" + currentSlotArgString + ")";
        //Declare slot
        headerFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "void " << currentSlotSignatureWithoutReturnTypeOrEndingSemicolon << ";" << endl;
        //Define slot
        sourceFileTextStream    << "void " << ClassName << "::" << currentSlotSignatureWithoutReturnTypeOrEndingSemicolon << endl
                                << "{" << endl;
        Q_FOREACH(IDesignEqualsImplementationStatement *currentSlotCurrentStatement, currentSlot->OrderedListOfStatements)
        {
            sourceFileTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentSlotCurrentStatement->toRawCppWithEndingSemicolon() << endl;
        }
        sourceFileTextStream    << "}" << endl;
    }

    //Header's footer
    headerFileTextStream    << "};" << endl
                            << endl
                            << "#endif // " << myNameHeaderGuard << endl;
    return true;
}
DesignEqualsImplementationClass::~DesignEqualsImplementationClass()
{
    qDeleteAll(Properties);
    Q_FOREACH(PrivateMemberType currentMember, PrivateMembers)
    {
        delete currentMember.second;
    }
    qDeleteAll(PrivateMethods);
    qDeleteAll(Slots);
    qDeleteAll(Signals);
}
QList<QString> DesignEqualsImplementationClass::allMyAvailableMemberGettersWhenInAnyOfMyOwnSlots_AsString()
{
    //Properties and PrivateMembers
    QList<QString> ret;

    //TODOreq: properties. i'm not too familiar with Q_PROPERTY, even though obviously i want to support it. i'm not sure, but i think i want the "read" part of the property here...

    Q_FOREACH(PrivateMemberType currentMember, PrivateMembers)
    {
        ret.append(currentMember.first);
    }
    return ret;
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
