#ifndef MOUSEANDORMOTIONVIEWMAKERGUI_H
#define MOUSEANDORMOTIONVIEWMAKERGUI_H

#include <QObject>

#include "objectonthreadhelper.h"
#include "mouseandormotionviewmaker.h"
#include "mouseandormotionviewmakerwidget.h"

class MouseAndOrMotionViewMakerGui : public QObject
{
    Q_OBJECT
public:
    explicit MouseAndOrMotionViewMakerGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<MouseAndOrMotionViewMaker> m_BusinessThread;
    MouseAndOrMotionViewMakerWidget m_Gui;
public slots:
    void handleMouseAndOrMotionViewMakerReadyForConnections();
    void handleAboutToQuit();
};

#endif // MOUSEANDORMOTIONVIEWMAKERGUI_H
