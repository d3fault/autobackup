#ifndef USERINTERFACESKELETONGENERATOR_H
#define USERINTERFACESKELETONGENERATOR_H

#include <QObject>

#include "userinterfaceskeletongeneratordata.h"

class QTextStream;

class IUserInterfaceImplStubGenerator;

class UserInterfaceSkeletonGenerator : public QObject
{
    Q_OBJECT
public:
    template<class T>
    static void establishConnectionsToAndFromBackendAndUi(UserInterfaceSkeletonGenerator *backend, T *ui)
    {
        connect(ui, &T::generateUserInterfaceSkeletonFromClassDeclarationStringRequested, backend, &UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromClassDeclarationString);
        connect(backend, &UserInterfaceSkeletonGenerator::e, ui, &T::handleE);
        connect(backend, &UserInterfaceSkeletonGenerator::o, ui, &T::handleO);
        connect(backend, &UserInterfaceSkeletonGenerator::finishedGeneratingUserInterfaceSkeleton, ui, &T::handleFinishedGeneratingUserInterfaceSkeleton);
    }
    static QString TAB;

    explicit UserInterfaceSkeletonGenerator(QObject *parent = 0);
    ~UserInterfaceSkeletonGenerator();
private:
    void displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData::Data &data);
    //void generatePureVirtualUserInterfaceHeaderFile(const UserInterfaceSkeletonGeneratorData::Data &data);
    void generateUserInterfaceImplStubsMaybe(const UserInterfaceSkeletonGeneratorData::Data &data, QList<QString> implStubShortNames);
    void generateAnyAndAllUserInterfaceImplStubs(const UserInterfaceSkeletonGeneratorData::Data &data);
    bool generateRequestResponseContractGlueMaybe_AndAddContractSignalsAndSlotsToData(UserInterfaceSkeletonGeneratorData::Data *data);

    QString m_OutputDir_WithTrailingSlash;
    QList<IUserInterfaceImplStubGenerator*> ImplStubGenerators;
    static QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        QString ret(inputString);
        ret.append("/");
        return ret;
    }
private:
    void populateDataUsingHardCodedCppXD(UserInterfaceSkeletonGeneratorData::Data &data);

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
signals:
    void e(QString msg);
    void o(QString msg);
    void finishedGeneratingUserInterfaceSkeleton(bool success);
public slots:
    void generateUserInterfaceSkeletonFromClassDeclarationString(const QString &classDeclarationCpp_ForParsing, QList<QString> implStubShortNames);
    void generateUserInterfaceSkeletonFromData(const UserInterfaceSkeletonGeneratorData::Data &data, QList<QString> implStubShortNames);
private slots:
    void handleFinishedGeneratingRequestResponseContractGlue(bool success);
};

#endif // USERINTERFACESKELETONGENERATOR_H
