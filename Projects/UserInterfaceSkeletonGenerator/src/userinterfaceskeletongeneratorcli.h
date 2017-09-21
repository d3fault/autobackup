#ifndef USERINTERFACESKELETONGENERATORCLI_H
#define USERINTERFACESKELETONGENERATORCLI_H

#include <QObject>

#include <QTextStream>

class UserInterfaceSkeletonGeneratorCli : public QObject
{
    Q_OBJECT
public:
    explicit UserInterfaceSkeletonGeneratorCli(QObject *parent = nullptr);
    void main();
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
signals:
    void generateUserInterfaceSkeletonFromClassDeclarationStringRequested(const QString & classDeclarationCpp_ForParsing, QList<QString> implStubShortNames);

    void exitRequested(int posixExitCode);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleFinishedGeneratingUserInterfaceSkeleton(bool success);
};

#endif // USERINTERFACESKELETONGENERATORCLI_H
