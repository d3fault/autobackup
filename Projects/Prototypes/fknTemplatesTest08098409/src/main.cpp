#include <QCoreApplication>

#include "gui.h"
#include "cli.h"
#include "backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Gui g;
    Cli c;
    Backend b;

    //int x;
    //b.connectToUi<int>(x);

    b.connectToUi<Gui>(&g);
    b.connectToUi<Cli>(&c);

    return 69;
}
