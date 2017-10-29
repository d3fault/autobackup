#ifndef WIDGETIMPLSTUBGENERATOR_H
#define WIDGETIMPLSTUBGENERATOR_H

#include "iuserinterfaceimplstubgenerator.h"

class WidgetImplStubGenerator : public IUserInterfaceImplStubGenerator
{
public:
    void generateImplStubFiles(const UserInterfaceSkeletonGeneratorData::Data &data, const QString &outputDirWithTrailingSlash) override;
private:
    void generateHeaderFile(const UserInterfaceSkeletonGeneratorData::Data &data, const QString &outputDirWithTrailingSlash);
    void generateSourceFile(const UserInterfaceSkeletonGeneratorData::Data &data, const QString &outputDirWithTrailingSlash);
protected:
    QString implStubClassSuffix() const override;
};

#endif // WIDGETIMPLSTUBGENERATOR_H
