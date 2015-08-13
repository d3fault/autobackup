#ifndef RENTALPROPERTYRETIREMENTCALCULATORWIDGET_H
#define RENTALPROPERTYRETIREMENTCALCULATORWIDGET_H

#include <QWidget>

class QLineEdit;

#define amountSavedPerMonthVar m_amountSavedPerMonthVar
#define downPaymentAmountForOneUnitVar m_downPaymentAmountForOneUnitVar
#define monthlyRentalProfitPerUnitVar m_monthlyRentalProfitPerUnitVar
#define targetMonthlyPassiveIncomeAmountToBeConsideredRetiredVar m_targetMonthlyPassiveIncomeAmountToBeConsideredRetiredVar

class RentalPropertyRetirementCalculatorWidget : public QWidget
{
    Q_OBJECT
public:
    RentalPropertyRetirementCalculatorWidget(QWidget *parent = 0);
private:
    QLineEdit *amountSavedPerMonthVar;
    QLineEdit *downPaymentAmountForOneUnitVar;
    QLineEdit *monthlyRentalProfitPerUnitVar;
    QLineEdit *targetMonthlyPassiveIncomeAmountToBeConsideredRetiredVar;

    bool parseLineEditTextAsNumber(double *number, QLineEdit *lineEdit);
signals:
    void calculateRentalPropertyRetirementRequested(double amountSavedPerMonth, double downPaymentAmountForOneUnit, double monthlyRentalProfitPerUnit, double targetMonthlyPassiveIncomeAmountToBeConsideredRetired);
public slots:
    void handleRetired(int numMonthsItTook);
private slots:
    void handleCalculateRentalPropertyRetirementButtonClicked();
};

#endif // RENTALPROPERTYRETIREMENTCALCULATORWIDGET_H
