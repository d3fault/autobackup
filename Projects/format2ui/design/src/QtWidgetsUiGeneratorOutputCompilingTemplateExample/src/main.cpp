#include <QApplication>

#include "qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h"
#include "../../shared/firstnamelastnameqobject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FirstNameLastNameQObject business;
    QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget gui;
    QObject::connect(&gui, &QtWidgetsUiGeneratorOutputCompilingTemplateExampleWidget::finishedCollectingUiVariables, &business, &FirstNameLastNameQObject::someSlot);
    gui.show();

    return a.exec();
}
