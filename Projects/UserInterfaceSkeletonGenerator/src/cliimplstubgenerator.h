#ifndef CLIIMPLSTUBGENERATOR_H
#define CLIIMPLSTUBGENERATOR_H

#include "iuserinterfaceimplstubgenerator.h"

class CliImplStubGenerator : public IUserInterfaceImplStubGenerator
{
public:
    void generateImplHeaderAndSourceStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash) override;
protected:
    QString implStubClassSuffix() override;
};

#endif // CLIIMPLSTUBGENERATOR_H
