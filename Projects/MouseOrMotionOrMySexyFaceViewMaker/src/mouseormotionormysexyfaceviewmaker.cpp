#include "mouseormotionormysexyfaceviewmaker.h"

#include <QScreen>
#include <QCursor>
#include <QGuiApplication>
#include <QImage>
#include <QRgb>
#include <QPainter>
#include <QProcessEnvironment>
#include <QStringList>
#include <QDebug>
#include <QTime>

#define MOUSE_OR_MOTION_OR_MY_SEXY_FACE_MINIMUM_TIME_AT_EACH_BEFORE_TOGGLING_MS 2000
#define MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL 4

#define MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter) \
QImage mySexyFaceImage((const unsigned char*)(m_LastReadFrameOfMySexyFace.constData()), m_CameraResolution.width(), m_CameraResolution.height(), QImage::Format_RGB32); \
QImage mySexyFaceImageThumbnail = mySexyFaceImage.scaled(m_CameraResolution.width() / MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL, m_CameraResolution.height() / MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL); \
int border = 20; \
painter.drawImage(m_ViewWidth-mySexyFaceImageThumbnail.width()-border, m_ViewHeight-mySexyFaceImageThumbnail.height()-border, mySexyFaceImageThumbnail); \


MouseOrMotionOrMySexyFaceViewMaker::MouseOrMotionOrMySexyFaceViewMaker(QObject *parent) :
    QObject(parent), m_Initialized(false), m_MousePixmapToDraw(":/mouseCursor.svg"), m_HaveFrameOfMySexyFace(false), m_ShowingMySexyFace(false) /*the worst default ever*/, m_TogglingMinimumsTimerIsStarted(false)
{
    m_Screen = QGuiApplication::primaryScreen();
    if(!m_Screen || (m_Screen->grabWindow(0).toImage().format() != QImage::Format_RGB32))
    {
        qDebug() << "failed to grab screen to verify it's format during MouseOrMotionOrMySexyFaceViewMaker construction";
        return;
    }

    m_Initialized = true;

    QSize screenResolution = m_Screen->size();
    m_ScreenResolutionX = screenResolution.width();
    m_ScreenResolutionY = screenResolution.height();

    connect(&m_IntervalTimer, SIGNAL(timeout()), this, SLOT(intervalTimerTimedOut()));

    m_FfMpegProcess.setReadChannel(QProcess::StandardOutput);
    connect(&m_FfMpegProcess, SIGNAL(readyRead()), this, SLOT(handleFfMpegStandardOutputReadyRead()));
    connect(&m_FfMpegProcess, SIGNAL(readyReadStandardError()), this, SLOT(handleFfmpegProcessError()));
    QProcessEnvironment ffmpegProcessEnvironenment = m_FfMpegProcess.processEnvironment();
    QString ldLibraryPath = ffmpegProcessEnvironenment.value("LD_LIBRARY_PATH");
    QString ldPathToMakeHighPriority = "/usr/local/lib";
    if(!ldLibraryPath.trimmed().isEmpty())
    {
        ldPathToMakeHighPriority = ldPathToMakeHighPriority + ":";
    }
    ldLibraryPath = ldPathToMakeHighPriority + ldLibraryPath;
    ffmpegProcessEnvironenment.insert("LD_LIBRARY_PATH", ldLibraryPath);
    m_FfMpegProcess.setProcessEnvironment(ffmpegProcessEnvironenment);
}
MouseOrMotionOrMySexyFaceViewMaker::~MouseOrMotionOrMySexyFaceViewMaker()
{
    if(m_FfMpegProcess.isOpen())
    {
        m_FfMpegProcess.close();
        m_FfMpegProcess.waitForFinished(-1);
    }
}
//TODOoptional: error quit if screen dimensions < view dimensions, for now it is simply undefined. I was tempted to write that the app would be pointless if that were the case, but actually once I implement "zoom", then it would still even be handy on 640x480 (working) -> 800x600 (view) cases (though why you wouldn't use 800x600 for working is beyond me)
QPoint MouseOrMotionOrMySexyFaceViewMaker::makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint)
{
    //start with a normal rectangle around the point
    QPoint ret(inputPoint.x()-(m_ViewWidth/2),
              inputPoint.y()-(m_ViewHeight/2));

    //then adjust
    if(ret.x() < 0)
    {
        //int adjust = (0-ret.x());
        ret.setX(0);
    }
    if(ret.y() < 0)
    {
        ret.setY(0);
    }
    if(ret.x() > (m_ScreenResolutionX-m_ViewWidth))
    {
        ret.setX(m_ScreenResolutionX-m_ViewWidth);
    }
    if(ret.y() > (m_ScreenResolutionY-m_ViewHeight))
    {
        ret.setY(m_ScreenResolutionY-m_ViewHeight);
    }
    return ret;
}
void MouseOrMotionOrMySexyFaceViewMaker::startMakingMouseOrMotionOrMySexyFaceViews(const QSize &viewSize, int updateInterval, int bottomPixelRowsToIgnore, const QString &cameraDevice, const QSize &cameraResolution)
{
    if(!m_Initialized)
    {
        qDebug() << "failed to initialize screen, or wrong image format from grabWindow";
        return;
    }

    m_ViewWidth = viewSize.width();
    m_ViewHeight = viewSize.height();
    m_IntervalTimer.start(updateInterval);
    m_BottomPixelRowsToIgnore = bottomPixelRowsToIgnore; //woot Xfce lets me specify this directly/easily

    m_CameraResolution = cameraResolution;

    QStringList ffmpegArguments;
    ffmpegArguments << "-loglevel" << "warning" << "-f" << "video4linux2" << "-s" <<  (QString::number(cameraResolution.width()) + QString("x") + QString::number(cameraResolution.height()));
    //need to upgrade my ffmpeg version: ffmpegArguments << "-aspect" << "3:2";
    ffmpegArguments << "-channel" << "1" << "-i" << cameraDevice << "-vcodec" << "rawvideo" << "-an" << "-r" << "21" << "-map" << "0" /*<< "-vf" << "crop=720:452:0:17"*/ << "-f" << "rawvideo" << "-pix_fmt" << "rgb32" << "-"; //QCamera is shit, libav too hard to use directly (isuck (actually i'd argue that that means the api sucks. cli args and c/c++ api should map 1:1 imotbh)). fuck yea one liners...
    m_FfMpegProcess.start("/usr/local/bin/ffmpeg", ffmpegArguments, QIODevice::ReadOnly);

    //m_BytesNeededForOneRawRGB32frame = cameraResolution.width() * cameraResolution.height() * 32; //TODOoptional: bpp should be any format QImage accepts, but really ffmpeg is pro as fuck at converting anything and everything into RGB32 :-D, so it's kind of a useless customization...
    m_BytesNeededForOneRawRGB32frame = cameraResolution.width() * cameraResolution.height() * 32;
    m_LastReadFrameOfMySexyFace.resize(m_BytesNeededForOneRawRGB32frame);
}
void MouseOrMotionOrMySexyFaceViewMaker::intervalTimerTimedOut()
{
    m_CurrentPixmap = QPixmap(); //memory optimization apparently

    //see if mouse position changed as optimization
    QPoint currentCursorPosition = QCursor::pos();
    bool mousePositionChanged = m_PreviousCursorPosition != currentCursorPosition;
    m_PreviousCursorPosition = currentCursorPosition;
    if(mousePositionChanged)
    {
        m_ShowingMySexyFace = false;
        m_TogglingMinimumsTimerIsStarted = false;

        //TODOreq: "pre-zoom" before grab

        //don't start grabbing to the left of, or above, the screen
        const QPoint &rectWithinResolution = makeRectAroundPointStayingWithinResolution(currentCursorPosition);
        //TODOqt4: QPixmap::grabWindow(), not sure if 'grabbing only primary screen' will work (and heck, surprised it's even working now in qt5)
        m_CurrentPixmap = m_Screen->grabWindow(0, //0 makes entire desktop eligible, restrained by a rect in following params
                                               rectWithinResolution.x(),
                                               rectWithinResolution.y(),
                                               m_ViewWidth, //my makeRect need only return topLeft point, fuck it
                                               m_ViewHeight);
        //draw mouse cursor
        {
            QPainter painter(&m_CurrentPixmap);
            painter.drawPixmap(currentCursorPosition.x()-rectWithinResolution.x(), currentCursorPosition.y()-rectWithinResolution.y(), m_MousePixmapToDraw.width(), m_MousePixmapToDraw.height(), m_MousePixmapToDraw);

            //draw my sexy face thumb
            if(m_HaveFrameOfMySexyFace)
            {
                MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter)
            }
        }
        emit presentPixmapForViewingRequested(m_CurrentPixmap);
        m_PreviousPixmap = QPixmap(); //motion detection requires two non-mouse-movement frames in a row. this makes our !isNull check below fail
        return;
    }

    //see if there was motion
    m_CurrentPixmap = m_Screen->grabWindow(0, 0, 0, m_ScreenResolutionX, m_ScreenResolutionY); //entire desktop
    if(!m_PreviousPixmap.isNull())
    {
        const QImage currentImage = m_CurrentPixmap.toImage();
        const QImage previousImage = m_PreviousPixmap.toImage(); //TODOreq: can optimize this re-using image from last time. should.
        //TODOoptimization: might be an optimization to compose current over previous using the xor thingo

        if(currentImage.size() == previousImage.size()) //don't crash/etc on resolution changes, just do nothing for now
        {
            const int currentImageWidth = currentImage.width();
            const int currentImageHeight = currentImage.height();

            QPoint firstDifferenceSeenPoint;

            //scan left right top down until difference seen. TODOoptimization: would definitely be more efficient to just center on the first difference seen, though on "window moving" we'd then see the top-left corner of the window being moved [only]. still, for typing etc it would suffice and save cpu. damnit, mfw window moves will trigger mouse movement code path anyways -_-
            bool differenceSeen = false;
            //what's easier? patching+recompiling qtcreator so that it doesn't show the line/col numbers (motion) at the top right corner (move it to bottom, or hide it altogether)... OR just scanning for motion from the bottom -> top instead of top -> bottom? ;-P.
            for(int j = ((currentImageHeight-1)-m_BottomPixelRowsToIgnore); j > -1; --j) //TO DOneoptional: chop out the 'start bar' from motion analysis, since my clock is updated every one second and i want to keep it that way...
            {
                const QRgb *currentPixelOfCurrentImage = (const QRgb*)(currentImage.constScanLine(j));
                const QRgb *currentPixelOfPreviousImage = (const QRgb*)(previousImage.constScanLine(j));
                for(int i = 0; i < currentImageWidth; ++i)
                {
                    if(*currentPixelOfCurrentImage != *currentPixelOfPreviousImage)
                    {
                        firstDifferenceSeenPoint = QPoint(i, j);
                        differenceSeen = true;
                        break;
                    }
                    ++currentPixelOfCurrentImage;
                    ++currentPixelOfPreviousImage;
                }
                if(differenceSeen)
                    break;
            }

            if(differenceSeen)
            {
                m_ShowingMySexyFace = false;
                m_TogglingMinimumsTimerIsStarted = false;

                //use first difference seen point
                const QPoint &rectWithinResolution = makeRectAroundPointStayingWithinResolution(firstDifferenceSeenPoint);
                QPixmap motionPixmap = m_CurrentPixmap.copy(rectWithinResolution.x(), rectWithinResolution.y(), m_ViewWidth, m_ViewHeight); //sure we used QImage for pixel analysis, but we still have the QPixmap handy so woot saved a conversion

                //draw my sexy face thumb
                if(m_HaveFrameOfMySexyFace)
                {
                    QPainter painter(&motionPixmap);
                    MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter)
                }

                emit presentPixmapForViewingRequested(motionPixmap);
            }
            else if(m_HaveFrameOfMySexyFace)
            {
                //TO DOnereq: a "minimum time with no differences seen before toggling" (and perhaps vice versa, but I'd say that one is less important (motion on desktop = jump back immediately makes sense to me)) check, so it doesn't bounce back and forth giving aneurisms. not implementing yet because it will just confuse me
                if(!m_ShowingMySexyFace)
                {
                    if(!m_TogglingMinimumsTimerIsStarted)
                    {
                        m_TogglingMinimumsElapsedTimer.start();
                        m_TogglingMinimumsTimerIsStarted = true;
                        return;
                    }
                    if(m_TogglingMinimumsElapsedTimer.elapsed() >= MOUSE_OR_MOTION_OR_MY_SEXY_FACE_MINIMUM_TIME_AT_EACH_BEFORE_TOGGLING_MS)
                    {
                        m_ShowingMySexyFace = true;
                        m_TogglingMinimumsTimerIsStarted = false; //not necessary since mouse/motion set to false...
                    }
                    else
                    {
                        return;
                    }
                }

                //present my sexy face. no desktop thumbnail for now, might change my mind on this later (pointless since it would be motionless and unreadable)
                QImage mySexyFaceImage((const unsigned char*)(m_LastReadFrameOfMySexyFace.constData()), m_CameraResolution.width(), m_CameraResolution.height(), QImage::Format_RGB32);
                QImage mySexyFaceImageMaybeScaled = mySexyFaceImage;
                if(m_CameraResolution.width() != m_ViewWidth || m_CameraResolution.height() != m_ViewHeight)
                {
                    mySexyFaceImageMaybeScaled = mySexyFaceImage.scaled(m_ViewWidth, m_ViewHeight);
                }
                QPixmap mySexyFacePixmap = QPixmap::fromImage(mySexyFaceImageMaybeScaled);
                emit presentPixmapForViewingRequested(mySexyFacePixmap);
            }
        }
    }
    m_PreviousPixmap = m_CurrentPixmap;

}
void MouseOrMotionOrMySexyFaceViewMaker::handleFfMpegStandardOutputReadyRead()
{
    //QByteArray readImageBuffer(bytesNeededForOneRawRGB32frame);
    bool atLeastOneRead = false;
    while(m_FfMpegProcess.bytesAvailable() >= m_BytesNeededForOneRawRGB32frame)
    {
        //read in all of them one frame at a time, even though we'll disregard all but the last
        qint64 amountRead = m_FfMpegProcess.read(m_LastReadFrameOfMySexyFace.data(), m_BytesNeededForOneRawRGB32frame);
        if(amountRead == m_BytesNeededForOneRawRGB32frame)
        {
            atLeastOneRead = true;
        }
    }
    if(atLeastOneRead)
    {
        //m_LastReadFrameOfMySexyFace = readImageBuffer;
        m_HaveFrameOfMySexyFace = true; //we COULD set this to false after it's presented, so we could stay in desktop mode or something if face cap process crashes etc... fuggit
    }
}
void MouseOrMotionOrMySexyFaceViewMaker::handleFfmpegProcessError()
{
    QByteArray allErrOut = m_FfMpegProcess.readAllStandardError();
    qDebug() << allErrOut;
}

