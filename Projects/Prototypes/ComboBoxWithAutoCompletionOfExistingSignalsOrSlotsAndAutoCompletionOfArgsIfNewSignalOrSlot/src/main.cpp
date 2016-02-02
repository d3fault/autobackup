#include <QApplication>

#include <QDebug>

#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"
#include "resultstatechangedlistener.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot w;
    if(!w.isInitialized())
        return 1;

    w.insertItems(w.count(), QStringList() << "someSlot0()" << "someSlot1(int someArg)");
    w.insertKnownTypes(QStringList() << "CustomType0" << "BlahType");

    QSize currentSize = w.size();
    currentSize.setWidth(400);
    w.resize(currentSize);

    ResultStateChangedListener resultStateChangedListener;
    QObject::connect(&w, SIGNAL(resultStateChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState)), &resultStateChangedListener, SLOT(handleResultStateChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState)));

    w.show();

    int retCode =  a.exec();

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState resultState = w.resultState();
    switch(resultState)
    {
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NoResult:
        qDebug() << "No result";
        break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ExistingResult:
        qDebug() << "Existing result:" << w.currentText(); //in d=i, the associated existing signal/slot is stored in the itemData at this index in the combo box, so no parsing is needed. it can be retrieved via: w.itemData(w.currentIndex())
        break;
    case ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::NewResult:
        qDebug() << "New result:" << w.currentText(); //in d=i, there is no itemData so we parse the text and try to create a new signal/slot out of it
        break;
    }

    return retCode;
}
