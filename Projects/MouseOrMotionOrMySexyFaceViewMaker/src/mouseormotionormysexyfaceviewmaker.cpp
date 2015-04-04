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
#include <QTimer>

#define MOUSE_OR_MOTION_OR_MY_SEXY_FACE_MINIMUM_TIME_MS_WITHOUT_MOTION_BEFORE_SHOWING_MY_FACE 2000
#define MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL 4
#define MOUSE_OR_MOTION_OR_MY_SEXY_FACE_ZOOM_FACTOR 2.0

#define MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter) \
QImage mySexyFaceImage((const unsigned char*)(m_LastReadFrameOfMySexyFace.constData()), m_CameraResolution.width(), m_CameraResolution.height(), QImage::Format_RGB32); \
QImage mySexyFaceImageThumbnail = mySexyFaceImage.scaled(m_CameraResolution.width() / MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL, m_CameraResolution.height() / MOUSE_OR_MOTION_OR_MY_SEXY_FACE_DIVIDE_MY_SEXY_FACE_BY_WHEN_MAKING_THUMBNAIL); \
int border = 20; \
painter.drawImage(m_ViewSize.width()-mySexyFaceImageThumbnail.width()-border, m_ViewSize.height()-mySexyFaceImageThumbnail.height()-border, mySexyFaceImageThumbnail); \

//we want the video to update on the thumbnail during the 2 seconds we wait (especially since there will be lots of tiny pauses (less than 2 seconds) during normal usage where we still want video to continue)
#define REDRAW_MY_FACE_THUMBNAIL_ON_CURRENTLY_PRESENTED_PIXMAP \
QPainter painter(&m_CurrentPixmapBeingPresented); \
MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter) \
emit presentPixmapForViewingRequested(m_CurrentPixmapBeingPresented);

//TODOreq: on the short term, the easiest way to get back to being able to cam my devbox/sexy-face is by modding this to save the "made view", via some cli arg or whatever. Not sure if I should mux the video file immediately or just save some PNGs and mux the video later (would be higher quality to mux LATER (not screencap my sexy face xD)). In any case, this solution has a "24 hour delay" involved, heh. But it still kinda relates to (and basically IS) that derived idea in the design folder... so doing this properly could take some time ffff
//TODOreq: use the "blinking cursor" graphic only as dimensions, and a cli arg to specify what color to fill it in with. the blinking cursor is styled the same way as the text in qt creator. of course, if no cli arg for the cursor color is specified, we should use the color in the graphic (black, the default, atm)
//^^I could also quite easily have two instances running, one on devbox and the other on online box, so I could finally cam online box actions. Just need to suck in the frames from the devbox and mux them all together (AGAIN, this is implicitly covered in the derived idea. guh, fingers vs. this (I guess I could do fingers for a little and THEN do this (except I want to cam creation of fingers (sounds like a familiar dependency that's cost me years of my life (*cough* Abc2whichHasntEarnedMeApenny *cough* (lel))))). Since I usually only use one box at a time (at least typing/mouse wise (sometimes compiling/etc on other machines), it works out nicely. My brain is NOT multi-core :(
//TODOoptional: the mouseOrMotion code could additionally (or solely) be serialized to create a timeline of "doing something on OS" ranges, and I could use those ranges to cut out the duplicate-and-shittier-quality 'my sexy face-ONLY' segments (which do still have justification for being made in general, since that's how I 'live stream'... but the non-OS portions of those segments don't have any need to be stored permanently)
MouseOrMotionOrMySexyFaceViewMaker::MouseOrMotionOrMySexyFaceViewMaker(QObject *parent)
    : QObject(parent)
    , m_ViewMode(MouseOrMotionOrMySexyFaceViewMode)
    , m_Initialized(false)
    , m_CaptureIntervalTimer(new QTimer(this))
    , m_MotionDetectionIntervalTimer(new QTimer(this))
    , m_MousePixmapToDraw(":/mouseCursor.svg")
    , m_ThereWasMotionRecently(false)
    //, m_DrawMySexyFaceFullscreenAsSoonAsFramesAreRead(false)
    , m_HaveFrameOfMySexyFace(false)
    , m_ShowingMySexyFace(false) /*the worst default ever*/
    , m_TogglingMinimumsTimerIsStarted(false)
    , m_VerifyEvery10secondsMySexyFaceStreamIsntFrozen_Timer(new QTimer(this))
{
    m_Screen = QGuiApplication::primaryScreen();
    if(!m_Screen || (m_Screen->grabWindow(0).toImage().format() != QImage::Format_RGB32))
    {
        qDebug() << "failed to grab screen to verify it's format during MouseOrMotionOrMySexyFaceViewMaker construction";
        return;
    }

    m_Initialized = true;

    QSize screenResolution = m_Screen->size();
    //qDebug() << screenResolution; //TODOoptional: if(numScreens > 1 && screen resolution == capture resolution, give qmessagebox asking yes/no for whether to continue. when i don't do xrandr, the secondary screen (as 'primaryScreen()' above) is used which fucks up royally the motion detection
    m_ScreenResolutionX = screenResolution.width();
    m_ScreenResolutionY = screenResolution.height();

    connect(m_CaptureIntervalTimer, SIGNAL(timeout()), this, SLOT(captureIntervalTimerTimedOut())); //this timer just captures/draws frames
    connect(m_MotionDetectionIntervalTimer, SIGNAL(timeout()), this, SLOT(motionDetectionIntervalTimerTimedOut())); //this timer tells the above timer what source of video to use (it relies on the captured data from above timer for analysis, but can hold onto copies to compare less often than presentation)
    //HOWEVER, since checking mouse motion is ridiculously cheap compared to checking image motion, we still check mouse motion at every capture/present interval

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

    connect(m_VerifyEvery10secondsMySexyFaceStreamIsntFrozen_Timer, SIGNAL(timeout()), this, SLOT(verifyMySexyFaceStreamIsntFrozen()));
    m_VerifyEvery10secondsMySexyFaceStreamIsntFrozen_Timer->start(10*1000);
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
QRect MouseOrMotionOrMySexyFaceViewMaker::makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint) //TODOreq: does this return the upper-left point of the rect instead? confused but eh methinks it works and i remember changing it to something like that
{
    QSize targetSize; //taking into account zoom factor
    targetSize.setWidth(m_ViewSize.width()/MOUSE_OR_MOTION_OR_MY_SEXY_FACE_ZOOM_FACTOR);
    targetSize.setHeight(m_ViewSize.height()/MOUSE_OR_MOTION_OR_MY_SEXY_FACE_ZOOM_FACTOR);

    //start with a normal calculation of the top-left corner (may be out of bounds)
    QPoint topLeft(inputPoint.x()-(targetSize.width()/2),
              inputPoint.y()-(targetSize.height()/2));

    //then adjust it to make sure it's within bounds
    if(topLeft.x() < 0)
    {
        //int adjust = (0-ret.x());
        topLeft.setX(0);
    }
    if(topLeft.y() < 0)
    {
        topLeft.setY(0);
    }
    if(topLeft.x() > (m_ScreenResolutionX-targetSize.width()))
    {
        topLeft.setX(m_ScreenResolutionX-targetSize.width());
    }
    if(topLeft.y() > (m_ScreenResolutionY-targetSize.height()))
    {
        topLeft.setY(m_ScreenResolutionY-targetSize.height());
    }

    QRect ret(topLeft.x(), topLeft.y(), targetSize.width(), targetSize.height());
    return ret;
}
void MouseOrMotionOrMySexyFaceViewMaker::drawMySexyFace()
{
    if(m_HaveFrameOfMySexyFace)
    {
        if(!m_ShowingMySexyFace && m_ViewMode != MySexyFaceViewMode)
        {
            if(!m_TogglingMinimumsTimerIsStarted)
            {
                m_TogglingMinimumsElapsedTimer.start();
                m_TogglingMinimumsTimerIsStarted = true;
                REDRAW_MY_FACE_THUMBNAIL_ON_CURRENTLY_PRESENTED_PIXMAP
                        return;
            }
            if(m_TogglingMinimumsElapsedTimer.elapsed() >= MOUSE_OR_MOTION_OR_MY_SEXY_FACE_MINIMUM_TIME_MS_WITHOUT_MOTION_BEFORE_SHOWING_MY_FACE)
            {
                m_ShowingMySexyFace = true;
                m_TogglingMinimumsTimerIsStarted = false; //not necessary since mouse/motion set to false...
            }
            else
            {
                //you might think the desktop should be redrawn before the my sexy face thumbnail is, but since nothing has changed that would be pointless
                REDRAW_MY_FACE_THUMBNAIL_ON_CURRENTLY_PRESENTED_PIXMAP
                //not enough time elaspsed
                return;
            }
        }

        //TODOoptimization: we re-convert the last read frame of my sexy face even if it hasn't changed (ie, our app's capture fps is higher than the capture CARD fps)

        //present my sexy face. no desktop thumbnail for now, might change my mind on this later (pointless since it would be motionless and unreadable)
        QImage mySexyFaceImage((const unsigned char*)(m_LastReadFrameOfMySexyFace.constData()), m_CameraResolution.width(), m_CameraResolution.height(), QImage::Format_RGB32);
        QImage mySexyFaceImageMaybeScaled = mySexyFaceImage;

        //COMMENT NEXT 3 LINES TO NOT SCALE 720x480 -> 800x600
        if(m_CameraResolution.width() != m_ViewSize.width() || m_CameraResolution.height() != m_ViewSize.height())
        {
            mySexyFaceImageMaybeScaled = mySexyFaceImage.scaled(m_ViewSize.width(), m_ViewSize.height());
        }

        QPixmap mySexyFacePixmap = QPixmap::fromImage(mySexyFaceImageMaybeScaled);
        emit presentPixmapForViewingRequested(mySexyFacePixmap);
    }
    //else: leave whatever is already drawn (TO DOnereq: app starts up and there is no motion. nothing is drawn (who cares tbh)?)
}
//this method depends on scanning from bottom to top, left to right
bool MouseOrMotionOrMySexyFaceViewMaker::thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(int verticalPositionOfMotionPoint, int horizontalPositionOfMotionPoint, int imageWidth)
{
    if((verticalPositionOfMotionPoint - m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height()) < 0)
    {
        return false;
    }
    if((horizontalPositionOfMotionPoint + m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.width()) > imageWidth)
    {
        return false;
    }
    return true;
}
void MouseOrMotionOrMySexyFaceViewMaker::thereWasMotionAt(QPoint point)
{
    m_LastPointWithMotionSeen = point;
    m_ShowingMySexyFace = false;
    m_TogglingMinimumsTimerIsStarted = false;
    m_ThereWasMotionRecently = true;
}
void MouseOrMotionOrMySexyFaceViewMaker::zoomInIfNeeded()
{
    if(MOUSE_OR_MOTION_OR_MY_SEXY_FACE_ZOOM_FACTOR != 1.0)
    {
        m_CurrentPixmapBeingPresented = m_CurrentPixmapBeingPresented.scaled(m_ViewSize);
    }
}
void MouseOrMotionOrMySexyFaceViewMaker::startMakingMouseOrMotionOrMySexyFaceViews(const QSize &viewSize, int captureFps, int motionDetectionFps, int bottomPixelRowsToIgnore, const QString &cameraDevice, const QSize &cameraResolution, int optionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied, const QString &qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack)
{
    if(!m_Initialized)
    {
        emit e("failed to initialize screen, or wrong image format from grabWindow");
        emit quitRequested();
        return;
    }
    if(QGuiApplication::screens().size() == 1)
    {
        emit e("there's only one screen");
        emit quitRequested();
        return;
    }
    if(optionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied > -1)
    {
        if(m_ScreenResolutionX != optionalRequiredPrimaryScreenWidth_OrNegativeOneIfNotSupplied)
        {
            emit e("primary screen resolution was not what was expected");
            emit quitRequested();
            return;
        }
    }

    m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.load(":/qtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.png");
    if(!qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack.isEmpty())
    {
        QColor newQtBlinkingCursorColor(qtBlinkingCursorColor_OrEmptyStringIfToLeaveBlack);
        if(!newQtBlinkingCursorColor.isValid())
        {
            emit e("WARNING: Your specified color for the blinking Qt Creator cursor was invalid. Leaving at default of black. This will probably cause the Qt Creator blinking cursor to be detected as motion");
        }
        else
        {
            m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.fill(newQtBlinkingCursorColor);
        }
    }

    m_ViewSize = viewSize;
    m_LastPointWithMotionSeen.setX(m_ViewSize.width()/2);
    m_LastPointWithMotionSeen.setY(m_ViewSize.height()/2);
    m_CaptureIntervalTimer->start(1000/captureFps);
    m_MotionDetectionIntervalTimer->start(1000/motionDetectionFps);
    m_BottomPixelRowsToIgnore = bottomPixelRowsToIgnore; //woot Xfce lets me specify this directly/easily

    m_CameraResolution = cameraResolution;

    QStringList ffmpegArguments;
    ffmpegArguments << "-loglevel" << "warning" << "-f" << "video4linux2" << "-s" <<  (QString::number(cameraResolution.width()) + QString("x") + QString::number(cameraResolution.height()));
    //need to upgrade my ffmpeg version: ffmpegArguments << "-aspect" << "3:2";    

    //no brightness double:
    //ffmpegArguments << "-channel" << "1" << "-i" << cameraDevice << "-vcodec" << "rawvideo" << "-an" << "-r" << "21" << "-map" << "0" /*<< "-vf" << "crop=720:452:0:17"*/ << "-f" << "rawvideo" << "-pix_fmt" << "rgb32" << "-"; //QCamera is shit, libav too hard to use directly (isuck (actually i'd argue that that means the api sucks. cli args and c/c++ api should map 1:1 imotbh)). fuck yea one liners...
    //brightness doubled:
    ffmpegArguments << "-channel" << "1" << "-i" << cameraDevice << "-vcodec" << "rawvideo" << "-an" << "-r" << "21" << "-map" << "0" << "-vf" << /*"lutyuv=val*2"*/ "lutyuv='y=gammaval(0.55)'" << "-f" << "rawvideo" << "-pix_fmt" << "rgb32" << "-"; //QCamera is shit, libav too hard to use directly (isuck (actually i'd argue that that means the api sucks. cli args and c/c++ api should map 1:1 imotbh)). fuck yea one liners...

    m_FfMpegProcess.start("/usr/local/bin/ffmpeg", ffmpegArguments, QIODevice::ReadOnly);

    //m_BytesNeededForOneRawRGB32frame = cameraResolution.width() * cameraResolution.height() * 32; //TODOoptional: bpp should be any format QImage accepts, but really ffmpeg is pro as fuck at converting anything and everything into RGB32 :-D, so it's kind of a useless customization...
    m_BytesNeededForOneRawRGB32frame = cameraResolution.width() * cameraResolution.height() * 32;
    m_LastReadFrameOfMySexyFace.resize(m_BytesNeededForOneRawRGB32frame);
}
void MouseOrMotionOrMySexyFaceViewMaker::setMouseOrMotionOrMySexyFaceViewMode()
{
    m_ViewMode = MouseOrMotionOrMySexyFaceViewMode;
}
void MouseOrMotionOrMySexyFaceViewMaker::setMySexyFaceViewMode()
{
    m_ViewMode = MySexyFaceViewMode;
}
void MouseOrMotionOrMySexyFaceViewMaker::captureIntervalTimerTimedOut()
{
    if(m_ViewMode == MySexyFaceViewMode)
    {
        drawMySexyFace();
        return;
    }

    m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion = QPixmap(); //memory optimization apparently
    //m_DrawMySexyFaceFullscreenAsSoonAsFramesAreRead = false;

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
        const QRect &rectWithinResolution = makeRectAroundPointStayingWithinResolution(currentCursorPosition);
        //TODOqt4: QPixmap::grabWindow(), not sure if 'grabbing only primary screen' will work (and heck, surprised it's even working now in qt5)
        m_CurrentPixmapBeingPresented = m_Screen->grabWindow(0, //0 makes entire desktop eligible, restrained by a rect in following params
                                               rectWithinResolution.x(),
                                               rectWithinResolution.y(),
                                               rectWithinResolution.width(), //my makeRect need only return topLeft point, fuck it
                                               rectWithinResolution.height());
        //draw mouse cursor (TODOoptional: svg cock, bonus points if it splooges when i click)
        {
            QPainter painter(&m_CurrentPixmapBeingPresented);
            painter.drawPixmap(currentCursorPosition.x()-rectWithinResolution.x(), currentCursorPosition.y()-rectWithinResolution.y(), m_MousePixmapToDraw.width(), m_MousePixmapToDraw.height(), m_MousePixmapToDraw);
        }
        zoomInIfNeeded();
        {
            QPainter painter(&m_CurrentPixmapBeingPresented);
            //draw my sexy face thumb
            if(m_HaveFrameOfMySexyFace)
            {
                MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter)
            }
        }
        emit presentPixmapForViewingRequested(m_CurrentPixmapBeingPresented);
        //m_PreviousDesktopCap_AsImage_ForMotionDetection = QImage(); //motion detection requires two non-mouse-movement frames in a row. this makes our !isNull check fail
        return;
    }

    //save most recent frame for motion detector to use, regardless of whether or not we present it
    m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion = m_Screen->grabWindow(0, 0, 0, m_ScreenResolutionX, m_ScreenResolutionY); //entire desktop

    //draw last motion detected area or my sexy face
    if(m_ThereWasMotionRecently)
    {
        //draw rect around the last point where motion was detected
        //use first difference seen point
        const QRect &rectWithinResolution = makeRectAroundPointStayingWithinResolution(m_LastPointWithMotionSeen);
        m_CurrentPixmapBeingPresented = m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion.copy(rectWithinResolution); //sure we used QImage for pixel analysis, but we still have the QPixmap handy so woot saved a conversion
        //m_PreviousPixmap = QPixmap();

        zoomInIfNeeded();

        //draw my sexy face thumb
        if(m_HaveFrameOfMySexyFace)
        {
            QPainter painter(&m_CurrentPixmapBeingPresented);
            MY_SEXY_FACE_THUMBNAIL_SNIPPET_KDSFJLSKDJF(painter)
        }
        emit presentPixmapForViewingRequested(m_CurrentPixmapBeingPresented);
    }
    else
    {
        drawMySexyFace();
        //m_DrawMySexyFaceFullscreenAsSoonAsFramesAreRead = true;
    }
}
void MouseOrMotionOrMySexyFaceViewMaker::motionDetectionIntervalTimerTimedOut()
{
    if(m_ViewMode == MySexyFaceViewMode)
        return;

    //see if there was motion
    QImage currentImage = m_CurrentDesktopCap_AsPixmap_ForMotionDetection_ButAlsoForPresentingWhenNotCheckingForMotion.toImage(); //TODOoptimization: might be worth it to compose current over previous using the xor thingo
    const QImage currentImageBeforeQtCreatorBlinkingCursorDrawnIn = currentImage;
    if(!m_PreviousDesktopCap_AsImage_ForMotionDetection.isNull())
    {
        if(currentImage.size() == m_PreviousDesktopCap_AsImage_ForMotionDetection.size()) //don't crash/etc on resolution changes, just do nothing for now
        {
            const int currentImageWidth = currentImage.width();
            const int currentImageHeight = currentImage.height();

            //scan left right bottom to top (used to be top to bottom, isn't anymore) until difference seen. TODOoptimization: would definitely be more efficient to just center on the first difference seen, though on "window moving" we'd then see the top-left corner of the window being moved [only]. still, for typing etc it would suffice and save cpu. damnit, mfw window moves will trigger mouse movement code path anyways -_-
            m_ThereWasMotionRecently = false;
            //what's easier? patching+recompiling qtcreator so that it doesn't show the line/col numbers (motion) at the top right corner (move it to bottom, or hide it altogether)... OR just scanning for motion from the bottom -> top instead of top -> bottom? ;-P.
            bool firstMotionDetected = false;
            for(int y = ((currentImageHeight-1)-m_BottomPixelRowsToIgnore); y > -1; --y)
            {
                QRgb *currentPixelOfCurrentImage = (QRgb*)(currentImage.scanLine(y));
                QRgb *currentPixelOfPreviousImage = (QRgb*)(m_PreviousDesktopCap_AsImage_ForMotionDetection.scanLine(y));
                for(int x = 0; x < currentImageWidth; ++x)
                {
                    if(*currentPixelOfCurrentImage != *currentPixelOfPreviousImage)
                    {
                        //static int count = 0;
                        if(!firstMotionDetected)
                        {
                            //find first motion, draw cursor on top (make sure there is space above and to the right of it, don't go out of bounds (if it would have been out of bounds, the motion is valid)).... then continue onto finding motion. when a second motion is detected, we know it is NOT the cursor. if no more motion is seen, it was only the cursor
                            firstMotionDetected = true;
                            if(thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(y, x, currentImageWidth))
                            {
                                //draw qt blinking cursor and keep checking for motion
                                //m_PreviousDesktopCap_AsImage_ForMotionDetection.save("/run/shm/beforeCursorDrawnIn_Previous-" + QString::number(count) + ".png");
                                //currentImage.save("/run/shm/beforeCursorDrawnIn_Current-" + QString::number(count) + ".png");
#if 1
                                {
                                    QPainter qtCreatorBlinkingCursorCurrentImagePainter(&currentImage);
                                    qtCreatorBlinkingCursorCurrentImagePainter.drawImage(x, (y-m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height())+1, m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks); //TODOoptional: the +1 accounts for an off by one, which may also be present in thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching
                                    //QImage previous2 = m_PreviousDesktopCap_AsImage_ForMotionDetection;
                                    //QPainter p2(&previous2);
                                    QPainter qtCreatorBlinkingCursorPreviousImagePainter(&m_PreviousDesktopCap_AsImage_ForMotionDetection);
                                    qtCreatorBlinkingCursorPreviousImagePainter.drawImage(x, (y-m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height())+1, m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks);
                                }
#endif
                                //m_PreviousDesktopCap_AsImage_ForMotionDetection.save("/run/shm/afterCursorDrawnIn_Previous-" + QString::number(count) + ".png");
                                //currentImage.save("/run/shm/afterCursorDrawnIn_Current-" + QString::number(count) + ".png");
                                //++count;
                            }
                            else
                            {
                                //qDebug() << "diff at" << count;
                                thereWasMotionAt(QPoint(x, y));
                                break;
                            }
                        }
                        else
                        {
                            //qDebug() << "diff at" << count << "--" << x << y;
                            thereWasMotionAt(QPoint(x, y));
                            break;
                        }
                    }
                    ++currentPixelOfCurrentImage;
                    ++currentPixelOfPreviousImage;
                }
                if(m_ThereWasMotionRecently)
                    break;
            }
        }
    }
    m_PreviousDesktopCap_AsImage_ForMotionDetection = currentImageBeforeQtCreatorBlinkingCursorDrawnIn;
}
void MouseOrMotionOrMySexyFaceViewMaker::handleFfMpegStandardOutputReadyRead()
{
    //QByteArray readImageBuffer(bytesNeededForOneRawRGB32frame);
    while(m_FfMpegProcess.bytesAvailable() >= m_BytesNeededForOneRawRGB32frame)
    {
        //read in all of them one frame at a time, even though we'll disregard all but the last
        qint64 amountRead = m_FfMpegProcess.read(m_LastReadFrameOfMySexyFace.data(), m_BytesNeededForOneRawRGB32frame);
        if(amountRead == m_BytesNeededForOneRawRGB32frame)
        {
            m_HaveFrameOfMySexyFace = true; //we COULD set this to false after it's presented, so we could stay in desktop mode or something if face cap process crashes etc... fuggit
            /*if(m_DrawMySexyFaceFullscreenAsSoonAsFramesAreRead || m_ViewMode == MySexyFaceViewMode)
            {
                drawMySexyFace();
            }*/
        }
    }
}
void MouseOrMotionOrMySexyFaceViewMaker::handleFfmpegProcessError()
{
    QByteArray allErrOut = m_FfMpegProcess.readAllStandardError();
    qDebug() << allErrOut;
}
void MouseOrMotionOrMySexyFaceViewMaker::verifyMySexyFaceStreamIsntFrozen()
{
    if(!m_HaveFrameOfMySexyFace)
        return; //this would mean it never started showing at beginning, which is something I'd see

    if(!m_FrameOfMySexyFaceRead10secondsAgo.isEmpty()) //empty on first pass
    {
        if(m_LastReadFrameOfMySexyFace == m_FrameOfMySexyFaceRead10secondsAgo)
        {
            emit detectedMySexyFaceStreamIsFrozen();
            m_VerifyEvery10secondsMySexyFaceStreamIsntFrozen_Timer->stop();
        }
    }
    m_FrameOfMySexyFaceRead10secondsAgo = m_LastReadFrameOfMySexyFace;
}

