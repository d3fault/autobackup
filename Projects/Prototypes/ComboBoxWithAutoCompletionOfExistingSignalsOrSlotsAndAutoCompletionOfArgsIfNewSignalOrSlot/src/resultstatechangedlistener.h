#ifndef RESULTSTATECHANGEDLISTENER_H
#define RESULTSTATECHANGEDLISTENER_H

#include <QObject>

#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"

class ResultStateChangedListener : public QObject
{
    Q_OBJECT
public:
    explicit ResultStateChangedListener(QObject *parent = 0);
public slots:
    //void handleResultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType newResultType);
    //void handleSyntaxIsValidChanged(bool syntaxIsValid);
    void handleSelectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType);
};

#endif // RESULTSTATECHANGEDLISTENER_H
