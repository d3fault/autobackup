#include <QCoreApplication>

#include "userinterfaceskeletongenerator.h"

//god mode
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //TODOreq: proper UserInterfaceSkeletonGeneratorCli use
    UserInterfaceSkeletonGenerator generator;
    generator.generateUserInterfaceSkeletonFromClassDeclarationString();

    return 0;
}
