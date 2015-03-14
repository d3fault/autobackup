#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H

#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QCursor>
#include <QProcess>
#include <QSize>
#include <QTime>

class QScreen;

class MouseOrMotionOrMySexyFaceViewMaker : public QObject
{
    Q_OBJECT
public:
    explicit MouseOrMotionOrMySexyFaceViewMaker(QObject *parent = 0);
    ~MouseOrMotionOrMySexyFaceViewMaker();
private:
    //Mouse Or Motion members
    enum ViewMode
    {
          MouseOrMotionOrMySexyFaceViewMode
        , LastMouseOrMotionViewMode
        , MySexyFaceViewMode
    };
    ViewMode m_ViewMode;
    bool m_Initialized;
    QTimer *m_CaptureIntervalTimer;
    QTimer *m_MotionDetectionIntervalTimer;
    QPoint m_PreviousCursorPosition;
    QScreen *m_Screen;
    int m_ScreenResolutionX;
    int m_ScreenResolutionY;
    QSize m_ViewSize;
    int m_BottomPixelRowsToIgnore;
    QImage m_PreviousDesktopCap_AsImage_ForMotionDetection;
    QPixmap m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion;
    QPixmap m_CurrentPixmapBeingPresented;
    QPixmap m_MousePixmapToDraw;
    QPoint m_LastPointWithMotionSeen;
    bool m_ThereWasMotionRecently;
    //bool m_DrawMySexyFaceFullscreenAsSoonAsFramesAreRead;
    QImage m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks;

    //ffmpeg members
    bool m_HaveFrameOfMySexyFace;
    QProcess m_FfMpegProcess;
    QByteArray m_LastReadFrameOfMySexyFace;
    QByteArray m_FrameOfMySexyFaceRead10secondsAgo;
    QSize m_CameraResolution;
    qint64 m_BytesNeededForOneRawRGB32frame;
    bool m_ShowingMySexyFace;
    bool m_TogglingMinimumsTimerIsStarted;
    QTime m_TogglingMinimumsElapsedTimer;
    QTimer *m_VerifyEvery10secondsMySexyFaceStreamIsntFrozen_Timer;

    //Mouse Or Motion methods
    QRect makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint);
    void drawMySexyFace();
    bool thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(int verticalPositionOfMotionPoint, int horizontalPositionOfMotionPoint, int imageWidth);
    void thereWasMotionAt(QPoint point);
    void zoomInIfNeeded();
signals:
    void e(const QString &msg);
    void presentPixmapForViewingRequested(const QPixmap &pixmapForViewing);
    void detectedMySexyFaceStreamIsFrozen();
    void quitRequested();
public slots:
    void startMakingMouseOrMotionOrMySexyFaceViews(const QSize &viewSize, int captureFps, int motionDetectionFps, int bottomPixelRowsToIgnore, const QString &cameraDevice, const QSize &cameraResolution, int optionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied);
    void setMouseOrMotionOrMySexyFaceViewMode();
    //void setMouseOrLastMouseViewMode();
    void setMySexyFaceViewMode();
    void captureIntervalTimerTimedOut();
    void motionDetectionIntervalTimerTimedOut();
private slots:
    void handleFfMpegStandardOutputReadyRead();
    void handleFfmpegProcessError();
    void verifyMySexyFaceStreamIsntFrozen();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H
