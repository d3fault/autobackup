#include <QApplication>

#include "antikeyandmouseloggerwidget.h"
#include "antikeyandmouselogger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AntiKeyAndMouseLogger anti;
    AntiKeyAndMouseLoggerWidget w;
    QObject::connect(&anti, SIGNAL(presentShuffledKeymapPageRequested(KeyMap)), &w, SLOT(presentShuffledKeymapPage(KeyMap)));
    QObject::connect(&w, SIGNAL(translateShuffledKeymapEntryRequested(QString)), &anti, SLOT(translateShuffledKeymapEntry(QString)));
    QObject::connect(&anti, SIGNAL(shuffledKeymapEntryTranslated(KeyMapEntry)), &w, SLOT(appendTranslatedKeymapEntryToPasswordLineEdit(KeyMapEntry)));
    anti.generateShuffledKeymapAndRequestPresentationOfFirstPage();
    w.show();

    return a.exec();
}
