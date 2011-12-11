#include <QtGui/QApplication>
#include "dinosauralphabetgamewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dinosaurAlphabetGameWidget w;
    w.show();

    return a.exec();
}
