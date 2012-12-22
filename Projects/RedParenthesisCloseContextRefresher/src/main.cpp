#include <QtGui/QApplication>
#include "redparenthesisclosecontextrefresherwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RedParenthesisCloseContextRefresherWidget w;
    w.show();
    
    return a.exec();
}
