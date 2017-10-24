#ifndef REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H
#define REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H

#include <QObject>

#include "userinterfaceskeletongeneratordata.h"

class RequstsResponseContractsGlueGenerator : public QObject
{
    Q_OBJECT
public:
    explicit RequstsResponseContractsGlueGenerator(QObject *parent = 0);
    bool generateRequstsResponseContractsGlue(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash);
private:
    bool generateBusinessObjectRequestResponseContractsHeaderFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectRequestResponseContractsSourceFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectRequestResponseContractsPriFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash);
    bool generateBusinessObjectSomeSlotRequestResponseHeaderFiles(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash);

    static QString firstLetterToUpper(const QString &inputString)
    {
        if(inputString.isEmpty())
            return inputString;
        QString ret(inputString);
        ret.replace(0, 1, ret.at(0).toUpper());
        return ret;
    }
    static QString firstLetterToLower(const QString &inputString)
    {
        if(inputString.isEmpty())
            return inputString;
        QString ret(inputString);
        ret.replace(0, 1, ret.at(0).toLower());
        return ret;
    }
    static QString TAB;
signals:
    void e(QString);
    void error(bool success = false); //emit on error only (it used to be my finished signal), and only false
};

#endif // REQUSTSRESPONSECONTRACTSGLUEGENERATOR_H
