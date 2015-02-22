#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H

#include <QObject>
#include <QStringList>
#include <QSize>

#include "objectonthreadhelper.h"
#include "mouseormotionormysexyfaceviewmaker.h"
#include "mouseormotionormysexyfaceviewmakerwidget.h"

class QMenu;

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
    int m_CaptureFps;
    int m_MotionDetectionFps;
    int m_BottomPixelRowsToIgnore;
    QString m_CameraDeviceName;
    QSize m_CameraResolution;
    //QMenu *m_SystemTrayIconMenu;
public slots:
    void handleMouseAndOrMotionViewMakerReadyForConnections();
    void handleAboutToQuit();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
