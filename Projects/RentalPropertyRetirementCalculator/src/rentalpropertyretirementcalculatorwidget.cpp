#include "rentalpropertyretirementcalculatorwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#define RentalPropertyRetirementCalculatorWidget_DERP_ROW(readableText, variablePart) \
QHBoxLayout *variablePart##Row = new QHBoxLayout(); \
QLabel *variablePart##Label = new QLabel(tr(readableText)); \
variablePart = new QLineEdit(); \
variablePart##Row->addWidget(variablePart##Label); \
variablePart##Row->addWidget(variablePart); \
myLayout->addLayout(variablePart##Row);

RentalPropertyRetirementCalculatorWidget::RentalPropertyRetirementCalculatorWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    RentalPropertyRetirementCalculatorWidget_DERP_ROW("Amount Saved Per Month:", amountSavedPerMonthVar)
    RentalPropertyRetirementCalculatorWidget_DERP_ROW("Down Payment Amount For One Unit", downPaymentAmountForOneUnitVar)
    RentalPropertyRetirementCalculatorWidget_DERP_ROW("Monthly Rental Profit Per Unit", monthlyRentalProfitPerUnitVar)
    RentalPropertyRetirementCalculatorWidget_DERP_ROW("Target Monthly Passive Income Amount To Be Considered Retired", targetMonthlyPassiveIncomeAmountToBeConsideredRetiredVar)

    QPushButton *calculateRentalPropertyRetirementButton = new QPushButton(tr("Calculate how long it will take to retire"));
    myLayout->addWidget(calculateRentalPropertyRetirementButton);

    setLayout(myLayout);

    connect(calculateRentalPropertyRetirementButton, SIGNAL(clicked()), this, SLOT(handleCalculateRentalPropertyRetirementButtonClicked()));
}
bool RentalPropertyRetirementCalculatorWidget::parseLineEditTextAsNumber(double *number, QLineEdit *lineEdit)
{
    bool convertOk = false;
    QString amountSavedPerMonthString = lineEdit->text();
    *number = amountSavedPerMonthString.toDouble(&convertOk);
    if(!convertOk)
        QMessageBox::critical(this, tr("Error"), "One of the line edits was not a number");
    return convertOk;
}
void RentalPropertyRetirementCalculatorWidget::handleRetired(int numMonthsItTook)
{
    QMessageBox::information(this, tr("Retirement"), tr("You would retire after: ") + QString::number(numMonthsItTook) + tr(" months"));
}
void RentalPropertyRetirementCalculatorWidget::handleCalculateRentalPropertyRetirementButtonClicked()
{
    double amountSavedPerMonth;
    if(!parseLineEditTextAsNumber(&amountSavedPerMonth, amountSavedPerMonthVar))
        return;
    double downPaymentAmountForOneUnit;
    if(!parseLineEditTextAsNumber(&downPaymentAmountForOneUnit, downPaymentAmountForOneUnitVar))
        return;
    double monthlyRentalProfitPerUnit;
    if(!parseLineEditTextAsNumber(&monthlyRentalProfitPerUnit, monthlyRentalProfitPerUnitVar))
        return;
    double targetMonthlyPassiveIncomeAmountToBeConsideredRetired;
    if(!parseLineEditTextAsNumber(&targetMonthlyPassiveIncomeAmountToBeConsideredRetired, targetMonthlyPassiveIncomeAmountToBeConsideredRetiredVar))
        return;
    emit calculateRentalPropertyRetirementRequested(amountSavedPerMonth, downPaymentAmountForOneUnit, monthlyRentalProfitPerUnit, targetMonthlyPassiveIncomeAmountToBeConsideredRetired);
}
