#include "resultstatechangedlistener.h"

#include <QDebug>

ResultStateChangedListener::ResultStateChangedListener(QObject *parent) :
    QObject(parent)
{ }
void ResultStateChangedListener::handleResultStateChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState newResultState)
{
    qDebug() << "Result state changed:" << newResultState;
}
