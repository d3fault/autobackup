#ifndef CLIIMPLSTUBGENERATOR_H
#define CLIIMPLSTUBGENERATOR_H

#include "iuserinterfaceimplstubgenerator.h"

class CliImplStubGenerator : public IUserInterfaceImplStubGenerator
{
public:
    void generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash) override;
private:
    void generateHeaderFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash);
    void generateSourceFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash);
    //void generateMainCppFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash);
protected:
    QString implStubClassSuffix() const override;
};

#endif // CLIIMPLSTUBGENERATOR_H
