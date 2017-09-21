#include "cliimplstubgenerator.h"

#include <QFile>
#include <QTextStream>

#include "userinterfaceskeletongenerator.h"

void CliImplStubGenerator::generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    generateHeaderFile(data, outputDirWithTrailingSlash);
    generateSourceFile(data, outputDirWithTrailingSlash);
    //TODOmb: generate main.cpp? it's small so why not <3?
}
void CliImplStubGenerator::generateHeaderFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".h");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in CliImplStubGenerator::generateHeaderFile");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);
    //data.gen-
    //so I guess the only reason I can't use "tell, don't ask" is because I'm implementing an interface and yea. well I mean I still could, but it wouldn't be worth it. would be too confusing

    generate_Q_OBJECT_inherittingClassHeader(data, textStream);
}
void CliImplStubGenerator::generateSourceFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".cpp");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in CliImplStubGenerator::generateSourceFile");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);

    generate_Q_OBJECT_inherittingStandardEmptyConstructorSourceCode(data, textStream);

    generateSignalHandlerSlotsSourceCode(data, textStream);
}
#if 0 //actually generating this doesn't make any sense or help at all. My workflow looks like this. launch Qt Creator -> create new cli project -> code backend -> run this app, parisng backend -> etc ..... note that during "create new cli project", main.cpp is already created for me! I guess if I wanted to gen a prepopulated one (calling connecToUi somewhere), that's a different story). idk about that yet tho
void CliImplStubGenerator::generateMainCppFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + "main.cpp");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in CliImplStubGenerator::generateMainCppFile");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);

    textStream << "#include <QCoreApplication>"
}
#endif
QString CliImplStubGenerator::implStubClassSuffix()
{
    return "Cli";
}
