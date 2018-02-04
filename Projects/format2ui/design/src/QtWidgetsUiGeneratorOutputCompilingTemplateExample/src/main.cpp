#include <QApplication>

#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"
#include "../../shared/firstnamelastnameqobject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget gui;
    if(!gui.parseArgs())
        return 1;
    FirstNameLastNameQObject business;
    QObject::connect(&gui, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::finishedCollectingUiVariables, &business, &FirstNameLastNameQObject::someSlot);
    gui.show();

    return a.exec();
}
