#ifndef WTNUMBERWATCHINGWIDGET_H
#define WTNUMBERWATCHINGWIDGET_H

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WBreak>
using namespace Wt;

#include <QString>
#include <QDebug>
#include <QMetaType>

#include "qtbroadcastservernumberwatcher.h"

class WtNumberWatchingWidget : public WApplication, public QtBroadcastServerNumberWatcher::QtBroadcastServerClient
{
public:
    WtNumberWatchingWidget(const WEnvironment& env);
private:
    WLineEdit *m_WatchNumberLineEdit;
    WContainerWidget *m_Canvas;
    void addNumberToWatch();
    void watchedNumberSeenUpdateGuiCallback(int number);

    virtual void finalize();
};

#endif // WTNUMBERWATCHINGWIDGET_H
