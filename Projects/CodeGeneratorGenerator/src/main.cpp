#include <QCoreApplication>

#include "codegeneratorgenerator.h"
#include "codegeneratorgeneratorcli.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CodeGeneratorGenerator generator;
    CodeGeneratorGeneratorCli cli;
    CodeGeneratorGenerator::establishConnectionsToAndFromBackendAndUi<CodeGeneratorGeneratorCli>(&generator, &cli);
    generator.generateCodeGenerator("/home/user/text/Projects/UserInterfaceSkeletonGenerator/design/src/RequestResponseContract2/src");

    return a.exec();
}
