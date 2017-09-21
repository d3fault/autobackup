#ifndef IUSERINTERFACEIMPLSTUBGENERATOR_H
#define IUSERINTERFACEIMPLSTUBGENERATOR_H

#include <QTextStream>

#include "userinterfaceskeletongenerator.h"
#include "userinterfaceskeletongeneratordata.h"

class IUserInterfaceImplStubGenerator
{
public:
    IUserInterfaceImplStubGenerator()=default;
    IUserInterfaceImplStubGenerator(const IUserInterfaceImplStubGenerator &other)=delete;
    virtual ~IUserInterfaceImplStubGenerator()=default;

    virtual void generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)=0;
protected:
    virtual QString implStubClassSuffix()=0;
    QString targetImplStubClassName(const QString &businessLogicClassName)
    {
        return businessLogicClassName + implStubClassSuffix();
    }
    void generateSignalsAndSlotsHeaderDeclarations(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream);
    void generate_Q_OBJECT_inherittingClassHeader(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream, QString directlyInherittedBaseClass = "QObject");

    void generate_Q_OBJECT_inherittingStandardEmptyConstructorSourceCode(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream, const QString &directlyInherittedBaseClass = "QObject");
    void generateSignalHandlerSlotsSourceCode(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream);

    QString tab = UserInterfaceSkeletonGenerator::TAB;
};

#endif // IUSERINTERFACEIMPLSTUBGENERATOR_H
