#include <QtGui/QApplication>

#include "clientdecentenoughackschemetest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientDecentEnoughAckSchemeTest test;
    Q_UNUSED(test);
    
    return a.exec();
}
