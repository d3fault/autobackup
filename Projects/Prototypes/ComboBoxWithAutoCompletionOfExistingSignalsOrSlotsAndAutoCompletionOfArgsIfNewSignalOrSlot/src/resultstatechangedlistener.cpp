#include "resultstatechangedlistener.h"

#include <QDebug>

ResultStateChangedListener::ResultStateChangedListener(QObject *parent) :
    QObject(parent)
{ }
void ResultStateChangedListener::handleResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType newResultType)
{
    qDebug() << "Result state changed:" << newResultType;
}
void ResultStateChangedListener::handleSyntaxIsValidChanged(bool syntaxIsValid)
{
    qDebug() << "Syntax is valid changed to:" << syntaxIsValid;
}
