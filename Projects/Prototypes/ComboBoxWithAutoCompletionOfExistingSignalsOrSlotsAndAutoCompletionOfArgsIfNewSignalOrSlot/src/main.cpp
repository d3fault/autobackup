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
    QObject::connect(&w, SIGNAL(selectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)), &resultStateChangedListener, SLOT(handleSelectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType)));

    w.show();

    return a.exec();
}
