#ifndef RENTALPROPERTYRETIREMENTCALCULATOR_H
#define RENTALPROPERTYRETIREMENTCALCULATOR_H

#include <QObject>

class RentalPropertyRetirementCalculator : public QObject
{
    Q_OBJECT
public:
    explicit RentalPropertyRetirementCalculator(QObject *parent = 0);
public slots:
    void calculateRentalPropertyRetirement(double amountSavedPerMonth, double downPaymentAmountForOneUnit, double monthlyRentalProfitPerUnit, double targetMonthlyPassiveIncomeAmountToBeConsideredRetired);
signals:
    void retired(int numMonthsItTook);
};

#endif // RENTALPROPERTYRETIREMENTCALCULATOR_H
