#include <QApplication>

#include "rentalpropertyretirementcalculatorgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RentalPropertyRetirementCalculatorGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
