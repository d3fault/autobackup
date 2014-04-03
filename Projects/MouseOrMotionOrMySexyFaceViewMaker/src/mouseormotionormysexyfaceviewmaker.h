#ifndef MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H
#define MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H

#include <QObject>
#include <QTimer>
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
    bool m_Initialized;
    QTimer m_IntervalTimer;
    QPoint m_PreviousCursorPosition;
    QScreen *m_Screen;
    int m_ScreenResolutionX;
    int m_ScreenResolutionY;
    int m_ViewWidth;
    int m_ViewHeight;
    int m_BottomPixelRowsToIgnore;
    QPixmap m_PreviousPixmapForMotionDetection;
    QPixmap m_CurrentPixmapForMotionDetection;
    QPixmap m_CurrentPixmapBeingPresented;
    QPixmap m_MousePixmapToDraw;

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

signals:
    void presentPixmapForViewingRequested(const QPixmap &);
public slots:
    void startMakingMouseOrMotionOrMySexyFaceViews(const QSize &viewSize, int updateInterval, int bottomPixelRowsToIgnore, const QString &cameraDevice, const QSize &cameraResolution);
    void intervalTimerTimedOut();
private slots:
    void handleFfMpegStandardOutputReadyRead();
    void handleFfmpegProcessError();
};

#endif // MOUSEORMOTIONORMYSEXYFACEVIEWMAKER_H
