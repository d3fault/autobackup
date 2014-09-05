#include "designequalsimplementationproject.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDataStream>

#include "designequalsimplementationprojectgenerator.h"

#define DesignEqualsImplementationProject_SERIALIZATION_VERSION 1

#define DesignEqualsImplementationProject_QDS(direction, qds, project) \
qds direction serializationVersion; \
qds direction project.Name; \
qds direction project.m_Classes; \
qds direction project.m_UseCases; \
qds direction project.m_DefinedElsewhereTypes; \
return qds;

DesignEqualsImplementationProject::DesignEqualsImplementationProject(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationProject::~DesignEqualsImplementationProject()
{
    qDeleteAll(m_Classes);
    qDeleteAll(m_UseCases);
}
DesignEqualsImplementationClass *DesignEqualsImplementationProject::createNewClass(const QString &newClassName, const QPointF &classPositionInGraphicsScene)
{
    DesignEqualsImplementationClass *newClass = new DesignEqualsImplementationClass(this);
    newClass->ClassName = newClassName;
    newClass->Position = classPositionInGraphicsScene;
    addClass(newClass);
    return newClass;
}
//cat(classes + definedElsewhereTypes); //maybe more types  of types (xD) to come
QList<QString> DesignEqualsImplementationProject::allKnownTypes() const
{
    QList<QString> ret;
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_Classes)
    {
        ret.append(currentClass->ClassName);
    }
    Q_FOREACH(const QString &currentDefinedElsewhereType, m_DefinedElsewhereTypes)
    {
        ret.append(currentDefinedElsewhereType);
    }
    return ret;
}
void DesignEqualsImplementationProject::addClass(DesignEqualsImplementationClass *newClass)
{
    connect(newClass, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    newClass->m_ParentProject = this;
    m_Classes.append(newClass);
    emit classAdded(newClass);
}
QList<DesignEqualsImplementationClass *> DesignEqualsImplementationProject::classes()
{
    return m_Classes;
}
#if 0
DesignEqualsImplementationClassInstance* DesignEqualsImplementationProject::createTopLevelClassInstance(DesignEqualsImplementationClass *classToMakeTopLevelInstanceOf)
{
    DesignEqualsImplementationClassInstance *classInstance = new DesignEqualsImplementationClassInstance(classToMakeTopLevelInstanceOf, 0, 0, "topLevel_" + classToMakeTopLevelInstanceOf->ClassName); //TODOreq: auto increment for top level auto vars
    m_TopLevelClassInstances.append(classInstance);
    return classInstance;
}
#endif
#if 0 //TODOinstancing
QList<DesignEqualsImplementationClassInstance*> DesignEqualsImplementationProject::topLevelClassInstances() //TODOreq: delete? maintain only list of use cases? it used to be a list of instances btw, before the refactor to 'this'/use-cases-root-class-lifeline
{
    return m_TopLevelClassInstances;
}
#endif
void DesignEqualsImplementationProject::addUseCase(DesignEqualsImplementationUseCase *newUseCase)
{
    //connect(newUseCase, SIGNAL(e);
    m_UseCases.append(newUseCase);
    emit useCaseAdded(newUseCase);
}
QList<DesignEqualsImplementationUseCase *> DesignEqualsImplementationProject::useCases()
{
    return m_UseCases;
}
void DesignEqualsImplementationProject::noteDefinedElsewhereType(const QString &definedElsewhereType)
{
    //note: at first i thought i'd need to 'ensure a defined elsewhere type is never noted more than once', BUT currently i am always building allKnownTypes using the defined elsewhere types, which means it would be impossible for me to detect a defined elsewhere type as a new type (which is prerequisite for me noting that it's defined elsewhere). still, a qset would de-dupe with ease if i ever can't make that guarantee in the future

    m_DefinedElsewhereTypes.append(definedElsewhereType); //TODOreq: [de-]serialize
}
QList<QString> DesignEqualsImplementationProject::definedElsewhereTypes() const
{
    return m_DefinedElsewhereTypes;
}
#if 0
void DesignEqualsImplementationProject::appendLineToTemporaryProjectGlueCode(const QString &line)
{
    m_TemporaryProjectGlueCodeLines.append(line);
}
#endif
bool DesignEqualsImplementationProject::writeTemporaryGlueCodeLines(const QString &destinationDirectoryPath)
{
    QString autoGlue = "AutoGlue_";
    QString autoGlueProjectName = autoGlue + Name;

    QString filenameToLowerWithoutExtension = autoGlue + Name;
    filenameToLowerWithoutExtension = filenameToLowerWithoutExtension.toLower();

    //autoglue_projectname.h
    QString projectGlueFilenameOnlyToLower = filenameToLowerWithoutExtension;
    QString projectGlueHeaderFilenameOnly = projectGlueFilenameOnlyToLower + ".h";
    QString projectGlueHeaderFilename = destinationDirectoryPath + projectGlueHeaderFilenameOnly;
    QFile projectGlueHeaderFile(projectGlueHeaderFilename);
    if(!projectGlueHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + projectGlueHeaderFilename);
        return false;
    }
    QTextStream projectGlueHeaderTextStream(&projectGlueHeaderFile);
    QString projectGlueHeadderGuard = Name.toUpper() + "_H";
    projectGlueHeaderTextStream << "#ifndef " << projectGlueHeadderGuard << endl;
    projectGlueHeaderTextStream << "#define " << projectGlueHeadderGuard << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "#include <QObject>" << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "class " << autoGlueProjectName << " : public QObject" << endl;
    projectGlueHeaderTextStream << "{" << endl;
    projectGlueHeaderTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl;
    projectGlueHeaderTextStream << "public:" << endl;
    projectGlueHeaderTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << autoGlueProjectName << "(QObject *parent = 0);" << endl;
    projectGlueHeaderTextStream << "};" << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "#endif // " << projectGlueHeadderGuard << endl;

    //autoglue_projectname.cpp
    QString projectGlueSourceFileName = filenameToLowerWithoutExtension + ".cpp";
    QFile projectGlueSourceFile(destinationDirectoryPath + projectGlueSourceFileName);
    if(!projectGlueSourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + projectGlueSourceFileName);
        return false;
    }
    QTextStream projectGlueSourceTextStream(&projectGlueSourceFile);
    projectGlueSourceTextStream << "#include \"" << projectGlueHeaderFilenameOnly << "\"" << endl;
    projectGlueSourceTextStream << endl;
    projectGlueSourceTextStream << autoGlueProjectName << "::" << autoGlueProjectName << "(QObject *parent)" << endl;
    projectGlueSourceTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl;
    projectGlueSourceTextStream << "{" << endl;
    //  Foo *foo = new Foo(this);"
#if 0
    Q_FOREACH(const QString &currentTempGlueLine, m_TemporaryProjectGlueCodeLines)
    {
        projectGlueSourceTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentTempGlueLine << endl; //TODOoptional: IStatement type shiz
    }
#endif
    projectGlueSourceTextStream << "}" << endl;

    return true;
}
//TODOreq: atomic project source generation. if it fails, no files are written/modified
bool DesignEqualsImplementationProject::generateSourceCodePrivate(ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath)
{
    //rambling: so basically our "Classes" member contains only the CORE classes of the project/design, but when we generate the source code we are still going to need to generate glue classes that own/instantiate/connect those core classes

    if(!allClassLifelinesInAllUseCasesInProjectHaveBeenAssignedInstances())
    {
        emit e("at least one class lifeline out of all your use cases in the project has not been assigned an instance"); //TODOoptional: tell them which one. TODOoptional: allow them to jump to it
        return false;
    }

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

    //TODOreq: should library generation include a .pro file? a .pri file? both? better solution in qbs? all of the above (default to which?)?

    DesignEqualsImplementationProjectGenerator projectGenerator(projectGenerationMode, destinationDirectoryPath);

#if 0 //TODOreq: merge with project generator
    if(projectGenerationMode != Library)
    {
        emit e("TODOreq: can only generate library for now");
        return false;

        //TODOreq: proper these hardcoded UI hacks
        //hardcoded main.cpp and *cli.h/cpp
        if(!tempGenerateHardcodedUiFiles(destinationDirectoryPath))
        {
            emit e("failed to generate temp hardcoded ui files in dir: " + destinationDirectoryPath);
            return false;
        }
    }
#endif

    //cleanupJitGeneratedLinesFromAPreviousGenerate();

    //two steps because use cases append connect statements to class constructors before writing
    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, m_Classes)
    {
        if(!projectGenerator.processClassStep0declaringClassInProject(designEqualsImplementationClass))
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX ", class step 0, " + designEqualsImplementationClass->ClassName);
            return false;
        }
    }
    Q_FOREACH(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, useCases())
    {
        if(!projectGenerator.processUseCase(designEqualsImplementationUseCase)) //bleh, async model breaks down here. can't [easily (code generator *cough* could do it)] be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX ", use case: " + designEqualsImplementationUseCase->Name);
            return false;
        }
    }

#if 0 //TODOreq maybe use this somewhere else or again later idfk
    if(!writeTemporaryGlueCodeLines(destinationDirectoryPath))
    {
        emit e("Failed to write temporary glue code for project. Check above for errors");
        return false;
    }
#endif

#if 0
    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, m_Classes)
    {
        //TODOreq: use case's exit signals should just be serialized/remembered. they are of design use, but of zero use when it comes to code generation (the signal is simply emitted). i was tempted to say "use case exit signals should generate temporary copies of the classes here, now with the exit signals inserted (temporary copies used to generate source code)", BUT as you can see that's wrong
        if(!projectGenerator.processClassStep1writingTheFile(designEqualsImplementationClass)) //bleh, async model breaks down here. can't be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX + ", class step 1, " + designEqualsImplementationClass->ClassName);
            return false;
        }
    }
#endif
    if(!projectGenerator.writeClassesToDisk())
    {
        emit e("failed to write project files to disk");
        return false;
    }

    //Q_FOREACH -- modifying use cases modifies the associated classes implicitly, so in theory there is no source to generate for use cases. HOWEVER, as of now the use cases are more focused on and contain the "full references" of the project as a whole, so generating (rather, trying to generate :-P) THAT should implicitly generate all classes (and more importantly, all parts of the class referenced in a use case)

    //TODOreq: proper design just jumped out at me: iterate the use cases at source generation time (NOW), but have the use cases do nothing more than populate the class diagram (ok ok, this is actually something that should happen any time class diagram view is lookeded at (or anything in the project changes if you don't do lazy loading)), THEN after use case iteratinos have spun class diagrams, outputting the classes by themselves is a breeze. however for now i'm going to try to just stick with focusing on use cases specifically, because i can't figure out where in a "Class" the "Ordered Use Case Events" would go

    //TODOreq: just like a use case needed an initial slot, so too does a project need an initial use case. It makes sense that if there is only 1x use case, that it is the "default" when invoked via CLI. If, for example, there are 2+ use cases, then the CLI wouldn't know which to use as the default (we could opt towards using the "first added", but that is dumb imo and i think actually that a "project::setDefaultUseCase" (already added) makes sense. Without setting said default, the CLI version should do NOTHING [unless cli arg switches indicate a use case], and the GUI version has the option of doing none or all. (two line edits, two push buttons, one shared qlineedit for output OR SOMETHING)

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
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QTextStream m_StdOut;" << endl;

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

#if 0 //TODOreq: oops, seems *cli.h/cpp aren't as static as I thought. I think when generating I need to generate the least dependent class first. Should I do dependency resolution (Bar depends on (is a child of) Foo, but Foo depends on nobody, therefore Foo goes in this Cli class), or should I just choose whatever class+slot is used as the [default] entry point (which, at the time of writing, is simply the first added use case event)? The latter is obviously easier, BUT I think I'm going to need to do some kind of dependency resolution anyways to determine the optimal (read: "latest as possible" (but not doing "jit instantiation" for now)) position for Bar to be instantiated (Foo's constructor in this case, BUT the app would work the exact fucking same if Foo/Bar were neighbors (and in that case, Bar would need to be in this Cli class's constructor)
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
bool DesignEqualsImplementationProject::allClassLifelinesInAllUseCasesInProjectHaveBeenAssignedInstances()
{
    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, useCases())
    {
        if(!currentUseCase->allClassLifelinesInUseCaseHaveBeenAssignedInstances())
        {
            emit e("a class class lifeline in use case '" + currentUseCase->Name + "' was not assigned an instance");
            return false;
        }
    }
    return true;
}
#if 0
void DesignEqualsImplementationProject::cleanupJitGeneratedLinesFromAPreviousGenerate()
{
    m_TemporaryProjectGlueCodeLines.clear();
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_Classes)
    {
        currentClass->cleanupJitGeneratedLinesFromAPreviousGenerate();
    }
}
#endif
void DesignEqualsImplementationProject::emitAllClassesAndUseCasesInProject()
{
#if 0
    //TODOreq: the signals will be emitted twice when LOADING a file (once when each individual deserialized 'thing' is added, and once again now). i'm not sure if 'some' of the later ones in the first batch might in fact be received, or if all of the first batch are definitely discarded (ui not listening yet)
    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, m_UseCases)
    {
        emit useCaseAdded(currentUseCase);
    }
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, classes())
    {
        emit classAdded(currentClass);
    }
#endif
}
void DesignEqualsImplementationProject::handleAddUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position)
{
    switch(umlItemType)
    {
    case DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS:
    {
        createNewClass("NewClass1" /*TODOreq: auto incrementing (but they'll likely change it anyways)*/, position);
    }
        break;
    case DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS_INTERFACE:
    {
        //TODOreq
    }
        break;
    }
}
void DesignEqualsImplementationProject::handleNewUseCaseRequested()
{
    DesignEqualsImplementationUseCase *useCase = new DesignEqualsImplementationUseCase(this, this);
    useCase->Name = Name + " - Use Case 1"; //TODOreq: auto incrementing
    addUseCase(useCase);
}
//TODOreq: overwrite warnings, backing up user modifications, putting "do not edit this file" warnings, etc etc
//TODOreq: enum to tell what kind of source to generate (cli, gui, lib, potentially 'web' eventually)
void DesignEqualsImplementationProject::generateSourceCode(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath)
{
    emit sourceCodeGenerated(generateSourceCodePrivate(projectGenerationMode, appendSlashIfNeeded(destinationDirectoryPath)));
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion = DesignEqualsImplementationProject_SERIALIZATION_VERSION;
    DesignEqualsImplementationProject_QDS(<<, out, project);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion;
    DesignEqualsImplementationProject_QDS(>>, in, project);
}
