#include <QCoreApplication>

#include "qtcliuigeneratoroutputcompilingtemplateexample.h"
#include "debugoutput.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtCliUiGeneratorOutputCompilingTemplateExample cli;
    DebugOutput debugOutput(&cli);
    cli.collectUiVariables();

    return a.exec();
}
