#include <QtGui/QApplication>

#include "easytreehashdiffanalyzergui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EasyTreeHashDiffAnalyzerGui app;
    Q_UNUSED(app)
    
    return a.exec();
}
