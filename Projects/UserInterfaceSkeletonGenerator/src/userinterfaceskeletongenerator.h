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
    void displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData &data);
    //void generatePureVirtualUserInterfaceHeaderFile(const UserInterfaceSkeletonGeneratorData &data);
    void generateUserInterfaceImplStubsMaybe(const UserInterfaceSkeletonGeneratorData &data, QList<QString> implStubShortNames);
    void generateAnyAndAllUserInterfaceImplStubs(const UserInterfaceSkeletonGeneratorData &data);

    QString m_OutputDirWithTrailingSlash;
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
    void populateDataUsingHardCodedCppXD(UserInterfaceSkeletonGeneratorData &data);
signals:
    void e(QString msg);
    void o(QString msg);
    void finishedGeneratingUserInterfaceSkeleton(bool success);
public slots:
    void generateUserInterfaceSkeletonFromClassDeclarationString(const QString &classDeclarationCpp_ForParsing, QList<QString> implStubShortNames);
    void generateUserInterfaceSkeletonFromData(const UserInterfaceSkeletonGeneratorData &data, QList<QString> implStubShortNames);
};

#endif // USERINTERFACESKELETONGENERATOR_H
