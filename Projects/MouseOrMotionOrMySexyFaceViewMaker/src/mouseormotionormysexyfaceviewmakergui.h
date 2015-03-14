#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H

#include <QObject>
#include <QStringList>
#include <QSize>
#include <QScopedPointer>

#include "mouseormotionormysexyfaceviewmaker.h"
#include "mouseormotionormysexyfaceviewmakerwidget.h"

class QMenu;

class MouseOrMotionOrMySexyFaceViewMakerGui : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotionOrMySexyFaceViewMakerGui(QObject *parent = 0);
private:
    QScopedPointer<MouseOrMotionOrMySexyFaceViewMakerWidget> m_Gui;
    int m_OptionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied;
    QSize m_ViewSize;
    int m_CaptureFps;
    int m_MotionDetectionFps;
    int m_BottomPixelRowsToIgnore;
    QString m_CameraDeviceName;
    QSize m_CameraResolution;
    //QMenu *m_SystemTrayIconMenu;
public slots:
    void handleMouseAndOrMotionViewMakerReadyForConnections(QObject *mouseOrMotionOrMySexyFaceViewMakerAsQObject);
    void handleE(const QString &msg);
    void handleQuitRequested();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKERGUI_H
