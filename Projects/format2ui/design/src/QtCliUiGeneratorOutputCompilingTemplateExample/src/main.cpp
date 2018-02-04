#include <QCoreApplication>

#include "qtcliuigeneratoroutputcompilingtemplateexample.h"
#include "../../shared/firstnamelastnameqobject.h"

//TODOreq: share arg _parsing_ with other ui (but maybe don't)
//TODOreq: parse and _use_ args. --firstname Steven should, for example, set it up so that the user need only press <Enter> to accept the prepopulated First Name of Steven. in the QtWidgets variant, we'd do lineEdit->setText(parsedArgForFirstName) in order to use it as the default
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
