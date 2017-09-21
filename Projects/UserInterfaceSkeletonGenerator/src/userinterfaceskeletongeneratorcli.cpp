#include "userinterfaceskeletongeneratorcli.h"

#include <QCoreApplication>

//TODOreq: eventually I'll give this app a GUI. I should use this app to generate it's own GUI [stub] <3. mindfuck x3 <3 I have a huge boner right now
UserInterfaceSkeletonGeneratorCli::UserInterfaceSkeletonGeneratorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    connect(this, &UserInterfaceSkeletonGeneratorCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOmb: generate this exitRequested shiz into our stubs that THIS APP generates
}
void UserInterfaceSkeletonGeneratorCli::main()
{
    emit generateUserInterfaceSkeletonFromClassDeclarationStringRequested("TODOreq", QList<QString>() << "cli" << "widget");
}
void UserInterfaceSkeletonGeneratorCli::handleE(QString msg)
{
    m_StdErr << "error: " << msg << endl;
}
void UserInterfaceSkeletonGeneratorCli::handleO(QString msg)
{
    m_StdOut << msg << endl;
}
void UserInterfaceSkeletonGeneratorCli::handleFinishedGeneratingUserInterfaceSkeleton(bool success)
{
    if(success)
    {
        handleO("UserInterfaceSkeletonGeneratorCli Success!");
        emit exitRequested(0);
    }
    else
    {
        handleE("UserInterfaceSkeletonGeneratorCli failed.");
        emit exitRequested(1);
    }
}
