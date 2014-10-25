#include "osiosinnetworkverifiedgraphicsviewwidget.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPicture>
#include <QBuffer>

OsiosInNetworkVerifiedGraphicsViewWidget::OsiosInNetworkVerifiedGraphicsViewWidget(QWidget *parent)
    : QWidget(parent)
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QPainter painter;
    QPicture picture;
    painter.begin(&picture);
    //painter.drawLine(0, 0, 5, 5);

    //TODOreq: put Osios in a graphics scene (i think this will work out of the box). i might need to choose a theme (fusion imo) to make the results consistent across platforms. i might need to do more, which might indicate i need to make a gui framework that is exact. make it out of raw rectangles etc. that would suck and it would probably be ugly

    view.render(&painter);
    painter.end();
    QByteArray myBufferContentsByteArray;
    QBuffer myBufferContents(&myBufferContentsByteArray);
    picture.save(&myBufferContents);
    //TODOreq: verify myBufferContents matches the other two nodes
    //TODOreq: the data is serialized right when it is received (generated?), but there is still a verification period until all 3 have stated that they have received+serialized it. that same verification need not be serialized to be trusted
}
