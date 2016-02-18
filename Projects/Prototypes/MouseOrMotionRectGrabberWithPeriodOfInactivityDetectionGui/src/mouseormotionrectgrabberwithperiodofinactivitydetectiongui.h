#ifndef MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONGUI_H
#define MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONGUI_H

#include <QObject>

#include <QScopedPointer>

#include "mouseormotionrectgrabberwithperiodofinactivitydetectionwidget.h"

class MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionGui(QObject *parent = 0);
private:
    QScopedPointer<MouseOrMotionRectGrabberWithPeriodOfInactivityDetectionWidget> m_Gui;
private slots:
    void mouseOrMotionRectGrabberWithPeriodOfInactivityDetectionReadyForConnections(QObject *mouseOrMotionRectGrabberWithPeriodOfInactivityAsQObject);
};

#endif // MOUSEORMOTIONRECTGRABBERWITHPERIODOFINACTIVITYDETECTIONGUI_H
