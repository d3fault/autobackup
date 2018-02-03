#include <QCoreApplication>

#include "qtcliuigeneratoroutputcompilingtemplateexample.h"
#include "../../shared/firstnamelastnameqobject.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FirstNameLastNameQObject business;
    QtCliUiGeneratorOutputCompilingTemplateExample cli;
    QObject::connect(&cli, &QtCliUiGeneratorOutputCompilingTemplateExample::finishedCollectingUiVariables, &business, &FirstNameLastNameQObject::someSlot);
    cli.collectUiVariables();

    return a.exec();
}
