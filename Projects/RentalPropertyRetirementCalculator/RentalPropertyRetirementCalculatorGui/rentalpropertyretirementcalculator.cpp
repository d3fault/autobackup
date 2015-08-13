#include "rentalpropertyretirementcalculator.h"

RentalPropertyRetirementCalculator::RentalPropertyRetirementCalculator(QObject *parent)
    : QObject(parent)
{ }
//TODOreq: one thing not factored in is the substantial increase in monthly rental profits after the mortgage period (usually 15 or 30 years) ends. So if the end result is greater than the mortgage period, know it is inaccurate and higher than it really would be
void RentalPropertyRetirementCalculator::calculateRentalPropertyRetirement(double amountSavedPerMonth, double downPaymentAmountForOneUnit, double monthlyRentalProfitPerUnit, double targetMonthlyPassiveIncomeAmountToBeConsideredRetired)
{
    double currentMonthlyPassiveIncomeAmount = 0; //fuck life
    int numMonths = 0;

    double currentAmountSavedForNextDownPayment = 0;

    while(currentMonthlyPassiveIncomeAmount < targetMonthlyPassiveIncomeAmountToBeConsideredRetired)
    {
        ++numMonths;
        currentAmountSavedForNextDownPayment += amountSavedPerMonth;
        currentAmountSavedForNextDownPayment += currentMonthlyPassiveIncomeAmount;
        while(currentAmountSavedForNextDownPayment >= downPaymentAmountForOneUnit)
        {
            currentAmountSavedForNextDownPayment -= downPaymentAmountForOneUnit;
            currentMonthlyPassiveIncomeAmount += monthlyRentalProfitPerUnit;
        }
    }
    emit retired(numMonths);
}
