#ifndef OSIOSMAINWINDOW_H
#define OSIOSMAINWINDOW_H

#include <QMainWindow>
#include "iosiosclient.h"
#include "iosioscopycatclient.h"

#include "osios.h"
#include "itimelinenode.h"

class QTabWidget;

class Osios;
class MainMenuActivityChangedTimelineNode;
class KeyPressInNewEmptyDocTimelineNode;

class OsiosMainWindow : public QMainWindow, public IOsiosClient /* the ONLY actual osios client. all other implementers (so far (one other actual client would be the cli app)) are simply relaying signals to this one */, public IOsiosCopycatClient /*sometimes. the interface includes enable/disabling of the mode (disabled by default)*/
{
    Q_OBJECT
public:
    OsiosMainWindow(Osios *osios, QWidget *parent = 0);
    QObject *asQObject();

    void setCopycatModeEnabled(bool enabled);
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    QTabWidget *m_MainMenuItemsTabWidget;

    bool m_CopycatModeEnabled;

    void addDockWidgets();

    //copycat
    void synthesizeMainMenuActivityTabChange(MainMenuActivityChangedTimelineNode *mainMenuTabChangedTimelineNode);
signals:
    void actionOccurred(TimelineNode action);
    void presentNotificationRequested(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel);

    //copycat enable/disable
    void connectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient *osiosCopycatClientToListenToSignalsFrom);
    void disconnectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient *osiosCopycatClientToStopListeningToSIgnalsFrom);
    //copycat individual events
    void synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode *keyPressInNewEmptyDocTimelineNodeTimelineNode);

#ifdef OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE
    void copycatTimelineNodeRendered(TimelineNode timelineNode, const QByteArray &renderedWidgetPngBytes);
#endif
public slots:
    void changeConnectionColor(int color);

    void synthesizeEventUsingTimelineNodeReceivedFromCopycatTarget(TimelineNode timelineNode);
private slots:
    void handleMainMenuItemsTabWidgetCurrentTabChanged();
};

#endif // OSIOSMAINWINDOW_H
