#include <QtGui/QApplication>
#include "lifeshaperstaticfilestest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LifeShaperStaticFilesTest test;
    Q_UNUSED(test)
    
    return a.exec();
}
