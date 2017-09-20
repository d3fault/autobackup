#ifndef IUSERINTERFACEIMPLSTUBGENERATOR_H
#define IUSERINTERFACEIMPLSTUBGENERATOR_H

#include "userinterfaceskeletongeneratordata.h"

class IUserInterfaceImplStubGenerator
{
public:
    IUserInterfaceImplStubGenerator()=default;
    IUserInterfaceImplStubGenerator(const IUserInterfaceImplStubGenerator &other)=delete;
    virtual ~IUserInterfaceImplStubGenerator()=default;

    virtual void generateImplHeaderAndSourceStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)=0;
protected:
    virtual QString implStubClassSuffix()=0;
    QString targetImplStubClassName(const QString &businessLogicClassName)
    {
        return businessLogicClassName + implStubClassSuffix();
    }
};

#endif // IUSERINTERFACEIMPLSTUBGENERATOR_H
