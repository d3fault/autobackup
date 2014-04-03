#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H

#include <QObject>
#include <QStringList>
#include <QSize>

#include "objectonthreadhelper.h"
#include "mouseormotionormysexyfaceviewmaker.h"
#include "mouseormotionormysexyfaceviewmakerwidget.h"

class MouseOrMotionOrMySexyFaceViewMakerGui : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotionOrMySexyFaceViewMakerGui(QObject *parent = 0);
    ~MouseOrMotionOrMySexyFaceViewMakerGui();
private:
    ObjectOnThreadHelper<MouseOrMotionOrMySexyFaceViewMaker> m_BusinessThread;
    MouseOrMotionOrMySexyFaceViewMakerWidget *m_Gui;
    QSize m_ViewSize;
    int m_UpdateIntervalMs;
    int m_BottomPixelRowsToIgnore;
    QString m_CameraDeviceName;
    QSize m_CameraResolution;
public slots:
    void handleMouseAndOrMotionViewMakerReadyForConnections();
    void handleAboutToQuit();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
