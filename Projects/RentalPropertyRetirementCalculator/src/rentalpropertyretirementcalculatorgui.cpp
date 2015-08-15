#include "rentalpropertyretirementcalculatorgui.h"

#include "rentalpropertyretirementcalculator.h"

RentalPropertyRetirementCalculatorGui::RentalPropertyRetirementCalculatorGui(QObject *parent)
    : QObject(parent)
{
    RentalPropertyRetirementCalculator *calculator = new RentalPropertyRetirementCalculator(this);
    m_Gui.reset(new RentalPropertyRetirementCalculatorWidget());

    connect(m_Gui.data(), SIGNAL(calculateRentalPropertyRetirementRequested(double,double,double,double)), calculator, SLOT(calculateRentalPropertyRetirement(double,double,double,double)));
    connect(calculator, SIGNAL(retired(int)), m_Gui.data(), SLOT(handleRetired(int)));

    m_Gui->show();
}
