#include <QApplication>

#include "socialnetworktimelinewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SocialNetworkTimelineWidget w;
    w.show();

    return a.exec();
}
