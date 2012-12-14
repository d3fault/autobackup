#include <QtGui/QApplication>
#include "endresulttargettest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EndResultTargetTest test(69, "boob");
    Q_UNUSED(test)
    
    return a.exec();
}
