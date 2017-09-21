#ifndef WIDGETIMPLSTUBGENERATOR_H
#define WIDGETIMPLSTUBGENERATOR_H

#include "iuserinterfaceimplstubgenerator.h"

class WidgetImplStubGenerator : public IUserInterfaceImplStubGenerator
{
public:
    void generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash) override;
private:
    void generateHeaderFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash);
    void generateSourceFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash);
protected:
    QString implStubClassSuffix() override;
};

#endif // WIDGETIMPLSTUBGENERATOR_H
