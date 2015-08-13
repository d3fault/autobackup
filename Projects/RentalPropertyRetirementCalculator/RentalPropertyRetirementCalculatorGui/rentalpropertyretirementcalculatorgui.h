#ifndef RENTALPROPERTYRETIREMENTCALCULATORGUI_H
#define RENTALPROPERTYRETIREMENTCALCULATORGUI_H

#include <QObject>
#include <QScopedPointer>

#include "rentalpropertyretirementcalculatorwidget.h"

class RentalPropertyRetirementCalculatorGui : public QObject
{
    Q_OBJECT
public:
    explicit RentalPropertyRetirementCalculatorGui(QObject *parent = 0);
private:
    QScopedPointer<RentalPropertyRetirementCalculatorWidget> m_Gui;
};

#endif // RENTALPROPERTYRETIREMENTCALCULATORGUI_H
