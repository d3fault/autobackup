#include <QApplication>

#include "socialnetworktimeline.h"
#include "socialnetworktimelinewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SocialNetworkTimeline timeline;
    SocialNetworkTimelineWidget ui;
    SocialNetworkTimeline::establishConnectionsToAndFromBackendAndUi<SocialNetworkTimelineWidget>(&timeline, &ui);

    ui.show();
    timeline.initializeSocialNetworkTimeline();

    return a.exec();
}
