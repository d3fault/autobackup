#include "widgetimplstubgenerator.h"

#include <QFile>
#include <QTextStream>

#include "userinterfaceskeletongenerator.h"

void WidgetImplStubGenerator::generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    generateHeaderFile(data, outputDirWithTrailingSlash);
    generateSourceFile(data, outputDirWithTrailingSlash);
    //TODOprobably: generate the "BusinessNameGui" class that is charged with performing the connections? I know I don't need to gen a main.cpp file here (unless I overwrite), but this is something else
}
void WidgetImplStubGenerator::generateHeaderFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".h");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in WidgetImplStubGenerator::generateHeaderFile");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);
    generate_Q_OBJECT_inherittingClassHeader(data, textStream, "QWidget");
}
void WidgetImplStubGenerator::generateSourceFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".cpp");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in WidgetImplStubGenerator::generateSourceFile");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);

    generate_Q_OBJECT_inherittingStandardEmptyConstructorSourceCode(data, textStream, "QWidget");

    generateSignalHandlerSlotsSourceCode(data, textStream);
}
QString WidgetImplStubGenerator::implStubClassSuffix() const
{
    return "Widget";
}
