#include "resultstatechangedlistener.h"

#include <QDebug>

ResultStateChangedListener::ResultStateChangedListener(QObject *parent) :
    QObject(parent)
{ }

void ResultStateChangedListener::handleSelectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType)
{
    switch(resultType)
    {
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoFunction:
        qDebug() << "No result selected";
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingFunction:
        qDebug() << "Existing result at index" << qobject_cast<ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot*>(sender())->currentIndex() << "selected";
    break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::TypedInFunction:
        ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot *sneder = qobject_cast<ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot*>(sender());
        qDebug() << "New result with" << (sneder->syntaxIsValid() ? "valid" : "invalid") << "syntax:" << sneder->parsedFunctionName();
        if(!sneder->parsedFunctionArguments().isEmpty())
            qDebug() << "It's args:" << sneder->parsedFunctionArguments();
    break;
    }
}
