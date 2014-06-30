#include "designequalsimplementationproject.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

#define DesignEqualsImplementationProject_SERIALIZATION_VERSION 1

#define DesignEqualsImplementationProject_QDS(direction, qds, project) \
qds direction project.Name; \
return qds;
//TODOreq:
//qds direction project.Classes; \
//qds direction project.UseCases; \

//new
DesignEqualsImplementationProject::DesignEqualsImplementationProject(QObject *parent)
    : QObject(parent)
{ }
//open
DesignEqualsImplementationProject::DesignEqualsImplementationProject(const QString &existingProjectFilename, QObject *parent)
    : QObject(parent)
{
    //TODOreq
}
DesignEqualsImplementationProject::~DesignEqualsImplementationProject()
{
    qDeleteAll(Classes);
    qDeleteAll(UseCases);
}
//TODOreq: atomic project source generation. if it fails, no files are written/modified
bool DesignEqualsImplementationProject::generateSourceCodePrivate(const QString &destinationDirectoryPath)
{
    //rambling: so basically our "Classes" member contains only the CORE classes of the project/design, but when we generate the source code we are still going to need to generate glue classes that own/instantiate/connect those core classes

    if(QFile::exists(destinationDirectoryPath))
    {
        emit e("destination directory must not exist: " + destinationDirectoryPath); //temp (very safe) fuck it for nows
        return false;
    }
    QDir destinationDir(destinationDirectoryPath);
    if(!destinationDir.mkpath(destinationDirectoryPath))
    {
        emit e("failed to make destination dir: " + destinationDirectoryPath);
        return false;
    }

    //TODOreq: proper these hardcoded UI hacks
    //hardcoded main.cpp, pro, and *cli.h/cpp
    if(!tempGenerateHardcodedUiFiles(destinationDirectoryPath))
    {
        emit e("failed to generate temp hardcoded ui files in dir: " + destinationDirectoryPath);
        return false;
    }

    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, Classes)
    {
        if(!designEqualsImplementationClass->generateSourceCode(destinationDirectoryPath)) //bleh, async model breaks down here. can't be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e("failed to generate source for: " + designEqualsImplementationClass->Name);
            return false;
        }
    }
    //Q_FOREACH -- modifying use cases modifies the associated classes implicitly, so there is no source to generate

    return true;
}
bool DesignEqualsImplementationProject::tempGenerateHardcodedUiFiles(const QString &destinationDirectoryPath)
{
    //main.cpp
    QString mainCppFilePath = destinationDirectoryPath + "main.cpp";
    QFile mainCppFile(mainCppFilePath, this);
    if(!mainCppFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + mainCppFilePath);
        return false;
    }
    QTextStream mainCppTextStream(&mainCppFile);
    mainCppTextStream << "#include <QCoreApplication>" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << "#include \"simpleasynclibraryslotinvokeandsignalresponsecli.h\"" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << "int main(int argc, char *argv[])" << endl;
    mainCppTextStream << "{" << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QCoreApplication a(argc, argv);" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << Name << "Cli cli;" << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_UNUSED(cli)" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "return a.exec();" << endl;
    mainCppTextStream << "}" << endl;

    //*cli.h
    QString cliHFilePath = destinationDirectoryPath + Name.toLower() + "cli.h";
    QFile cliHFile(cliHFilePath, this);
    if(!cliHFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + cliHFilePath);
        return false;
    }
    QTextStream cliHTextStream(&cliHFile);
    QString headerDoubleIncludeGuardDefine = Name.toUpper() + "CLI_H";
    cliHTextStream << "#ifndef " << headerDoubleIncludeGuardDefine << endl;
    cliHTextStream << "#define " << headerDoubleIncludeGuardDefine << endl;
    cliHTextStream << endl;
    cliHTextStream << "#include <QObject>" << endl;
    cliHTextStream << "#include <QTextStream>" << endl; //wat
    cliHTextStream << endl;
    QString cliClassName = Name + "Cli";
    cliHTextStream << "class " << cliClassName << " : public QObject" << endl;
    cliHTextStream << "{" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_" << "OBJECT" << endl;
    cliHTextStream << "public:" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << cliClassName << "(QObject *parent = 0);" << endl;
    cliHTextStream << "private:" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QTextStream m_StdOut;";

    //TODOreq: generic cli class
    //private slots:
    //  void handleFooSignal(bool success);

    cliHTextStream << "};" << endl;
    cliHTextStream << endl;
    cliHTextStream << "#endif // " << headerDoubleIncludeGuardDefine << endl;

    //*cli.cpp
    QString cliCppFilePath = destinationDirectoryPath + Name.toLower() + "cli.cpp";
    QFile cliCppFile(cliCppFilePath, this);
    if(!cliCppFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + cliCppFilePath);
        return false;
    }
    QTextStream cliCppTextStream(&cliCppFile);

#if 0 //TODOreq: oops, seems *cli.h/cpp aren't as static as I thought. I think when generating I need to generate the least dependent class first
#include "simpleasynclibraryslotinvokeandsignalresponsecli.h"

#include <QCoreApplication>

#include "../../foo.h"

//UI is not being factored into designEquals at this point, so this class is not even trying to be elegant/etc
SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::SimpleAsyncLibrarySlotInvokeAndSignalResponseCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    Foo *foo = new Foo(this);
    connect(foo, SIGNAL(fooSignal(bool)), this, SLOT(handleFooSignal(bool)));
    QMetaObject::invokeMethod(foo, "fooSlot", Q_ARG(QString, QString("test"))); //or could be a permanent connection with corresponding this.fooSlotInvocationRequested signal to start everything off
}
void SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::handleFooSignal(bool success)
{
    m_StdOut << QString("fooSignal indicated success = " + success ? "true" : "false") << endl;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

#endif

    return true;
}
//TODOreq: overwrite warnings, backing up user modifications, putting "do not edit this file" warnings, etc etc
void DesignEqualsImplementationProject::generateSourceCode(const QString &destinationDirectoryPath)
{
    emit sourceCodeGenerated(generateSourceCodePrivate(appendSlashIfNeeded(destinationDirectoryPath)));
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project)
{
    out << static_cast<quint8>(DesignEqualsImplementationProject_SERIALIZATION_VERSION);
    DesignEqualsImplementationProject_QDS(<<, out, project);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion;
    in >> serializationVersion;
    DesignEqualsImplementationProject_QDS(>>, in, project);
}
