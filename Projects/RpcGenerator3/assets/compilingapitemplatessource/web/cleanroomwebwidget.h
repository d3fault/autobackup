#include <Wt/WApplication>
using namespace Wt;
#ifndef CLEANROOMWEBWIDGET_H
#define CLEANROOMWEBWIDGET_H

#include <QStringList>
#include <QScopedPointer>

#include "cleanroomsession.h"

class ICleanRoom;

class CleanRoomWebWidget : public WApplication
{
public:
    static ICleanRoom *s_CleanRoom;

    CleanRoomWebWidget(const WEnvironment &myEnv, WtLibVersion version = WT_INCLUDED_VERSION);
    static WApplication *cleanRoomWebWidgetEntryPoint(const WEnvironment &myEnv);
private:
    friend class CleanRoomWeb;
    QScopedPointer<CleanRoomSession> m_Session;

    QList<WLabel*> m_CleanRoomDocsWidgets;

    void handleCleanRoomSessionStarted(CleanRoomSession session);

    void handleFrontPageDefaultViewReceived(QStringList frontPageDocs);
};

#endif // CLEANROOMWEBWIDGET_H
