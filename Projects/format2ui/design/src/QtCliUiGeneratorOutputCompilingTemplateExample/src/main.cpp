#include <QCoreApplication>

#include "qtcliuigeneratoroutputcompilingtemplateexample.h"
#include "../../shared/firstnamelastnameqobject.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtCliUiGeneratorOutputCompilingTemplateExample cli;
    if(!cli.parseArgs())
        return 1;
    FirstNameLastNameQObject business;
    QObject::connect(&cli, &QtCliUiGeneratorOutputCompilingTemplateExample::collectUiVariablesFinished, &business, &FirstNameLastNameQObject::someSlot);
    cli.collectUiVariables();

    return a.exec();
}
