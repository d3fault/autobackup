#include <QtGui/QApplication>
#include "dinosauralphabetgamewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<Qt::Key>("Qt::Key");

    dinosaurAlphabetGameWidget w;
    w.show();

    return a.exec();
}
