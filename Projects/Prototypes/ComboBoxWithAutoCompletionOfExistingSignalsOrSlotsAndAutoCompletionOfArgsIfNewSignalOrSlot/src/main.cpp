#include <QApplication>

#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot w;
    if(!w.isValid())
        return 1;

    w.insertItems(w.count(), QStringList() << "someSlot0()" << "someSlot1(int someArg)");
    w.insertKnownTypes(QStringList() << "CustomType0" << "BlahType");

    QSize currentSize = w.size();
    currentSize.setWidth(400);
    w.resize(currentSize);
    w.show();

    return a.exec();
}
