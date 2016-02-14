#include <QApplication>

#include "antikeyandmouseloggerwidget.h"
#include "antikeyandmouselogger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AntiKeyAndMouseLogger anti;
    AntiKeyAndMouseLoggerWidget w;
    QObject::connect(&anti, SIGNAL(shuffledKeymapGenerated(KeyMap)), &w, SLOT(useShuffledKeymap(KeyMap)));
    QObject::connect(&w, SIGNAL(translateShuffledKeymapEntryRequested(int)), &anti, SLOT(translateShuffledKeymapEntry(int)));
    QObject::connect(&anti, SIGNAL(shuffledKeymapEntryTranslated(KeyMapEntry)), &w, SLOT(appendTranslatedKeymapEntryToPasswordLineEdit(KeyMapEntry)));
    anti.generateShuffledKeymap();
    w.show();

    return a.exec();
}
