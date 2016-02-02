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
    void handleResultStateChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState newResultState);
};

#endif // RESULTSTATECHANGEDLISTENER_H
