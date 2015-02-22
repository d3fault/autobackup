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
    int m_ViewWidth;
    int m_ViewHeight;
    int m_BottomPixelRowsToIgnore;
    QImage m_PreviousDesktopCap_AsImage_ForMotionDetection;
    QPixmap m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion;
    QPixmap m_CurrentPixmapBeingPresented;
    QPixmap m_MousePixmapToDraw;
    QPoint m_LastPointWithMotionSeen;
    bool m_ThereWasMotionRecently;

    //ffmpeg members
    bool m_HaveFrameOfMySexyFace;
    QProcess m_FfMpegProcess;
    QByteArray m_LastReadFrameOfMySexyFace;
    QSize m_CameraResolution;
    qint64 m_BytesNeededForOneRawRGB32frame;
    bool m_ShowingMySexyFace;
    bool m_TogglingMinimumsTimerIsStarted;
    QTime m_TogglingMinimumsElapsedTimer;

    //Mouse Or Motion methods
    QPoint makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint);
    void drawMySexyFace();
signals:
    void presentPixmapForViewingRequested(const QPixmap &);
public slots:
    void startMakingMouseOrMotionOrMySexyFaceViews(const QSize &viewSize, int captureFps, int motionDetectionFps, int bottomPixelRowsToIgnore, const QString &cameraDevice, const QSize &cameraResolution);
    void setMouseOrMotionOrMySexyFaceViewMode(bool enabled);
    void setMouseOrLastMouseViewMode(bool enabled);
    void setMySexyFaceViewMode(bool enabled);
    void captureIntervalTimerTimedOut();
    void motionDetectionIntervalTimerTimedOut();
private slots:
    void handleFfMpegStandardOutputReadyRead();
    void handleFfmpegProcessError();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H
