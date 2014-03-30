#ifndef MOUSEANDORMOTIONVIEWMAKERGUI_H
#define MOUSEANDORMOTIONVIEWMAKERGUI_H

#include <QObject>
#include <QStringList>
#include <QSize>

#include "objectonthreadhelper.h"
#include "mouseandormotionviewmaker.h"
#include "mouseandormotionviewmakerwidget.h"

class MouseAndOrMotionViewMakerGui : public QObject
{
    Q_OBJECT
public:
    explicit MouseAndOrMotionViewMakerGui(QObject *parent = 0);
    ~MouseAndOrMotionViewMakerGui();
private:
    ObjectOnThreadHelper<MouseAndOrMotionViewMaker> m_BusinessThread;
    MouseAndOrMotionViewMakerWidget *m_Gui;
    QSize m_ViewSize;
    int m_UpdateIntervalMs;
    int m_BottomPixelRowsToIgnore;
public slots:
    void handleMouseAndOrMotionViewMakerReadyForConnections();
    void handleAboutToQuit();
};

#endif // MOUSEANDORMOTIONVIEWMAKERGUI_H
