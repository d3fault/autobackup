#include <QtGui/QApplication>

#include "symboliclinksdetectorgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SymbolicLinksDetectorGui symbolicLinksDetectorGui;
    Q_UNUSED(symbolicLinksDetectorGui)
    
    return a.exec();
}
