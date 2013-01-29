#include <QtGui/QApplication>

#include "workinginputtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WorkingInputTest test(69, "reverseMe");
    Q_UNUSED(test)
    
    return a.exec();
}
