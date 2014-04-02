#include "mouseandormotionviewmaker.h"

#include <QScreen>
#include <QCursor>
#include <QGuiApplication>
#include <QImage>
#include <QSize>
#include <QRgb>
#include <QPainter>

MouseAndOrMotionViewMaker::MouseAndOrMotionViewMaker(QObject *parent) :
    QObject(parent), m_Initialized(false), m_MousePixmapToDraw(":/mouseCursor.svg")
{
    m_Screen = QGuiApplication::primaryScreen();
    if(!m_Screen || (m_Screen->grabWindow(0).toImage().format() != QImage::Format_RGB32))
        return;

    m_Initialized = true;

    QSize screenResolution = m_Screen->size();
    m_ScreenResolutionX = screenResolution.width();
    m_ScreenResolutionY = screenResolution.height();

    connect(&m_IntervalTimer, SIGNAL(timeout()), this, SLOT(intervalTimerTimedOut()));
}
//TODOoptional: error quit if screen dimensions < view dimensions, for now it is simply undefined. I was tempted to write that the app would be pointless if that were the case, but actually once I implement "zoom", then it would still even be handy on 640x480 (working) -> 800x600 (view) cases (though why you wouldn't use 800x600 for working is beyond me)
QPoint MouseAndOrMotionViewMaker::makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint)
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
void MouseAndOrMotionViewMaker::startMakingMouseAndOrMotionViews(const QSize &viewSize, int updateInterval, int bottomPixelRowsToIgnore)
{
    if(!m_Initialized)
    {
        //emit d("failed to initialize screen, or wrong image format from grabWindow");
        return;
    }
    m_ViewWidth = viewSize.width();
    m_ViewHeight = viewSize.height();
    m_IntervalTimer.start(updateInterval);
    m_BottomPixelRowsToIgnore = bottomPixelRowsToIgnore; //woot Xfce lets me specify this directly/easily
}
void MouseAndOrMotionViewMaker::intervalTimerTimedOut()
{
    m_CurrentPixmap = QPixmap(); //memory optimization apparently

    //see if mouse position changed as optimization
    QPoint currentCursorPosition = QCursor::pos();
    bool mousePositionChanged = m_PreviousCursorPosition != currentCursorPosition;
    m_PreviousCursorPosition = currentCursorPosition;
    if(mousePositionChanged)
    {
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
                //use first difference seen point
                const QPoint &rectWithinResolution = makeRectAroundPointStayingWithinResolution(firstDifferenceSeenPoint);
                QPixmap motionPixmap = m_CurrentPixmap.copy(rectWithinResolution.x(), rectWithinResolution.y(), m_ViewWidth, m_ViewHeight); //sure we used QImage for pixel analysis, but we still have the QPixmap handy so woot saved a conversion
                emit presentPixmapForViewingRequested(motionPixmap);
            }
        }
    }
    m_PreviousPixmap = m_CurrentPixmap;
}
