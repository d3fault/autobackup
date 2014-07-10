#include "stevenhereandnowwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StevenHereAndNowWidget w;
    w.show();

    return a.exec();
}
