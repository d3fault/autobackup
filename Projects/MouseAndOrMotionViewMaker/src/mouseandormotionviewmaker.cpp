#include "mouseandormotionviewmaker.h"

#include <QScreen>
#include <QCursor>
#include <QGuiApplication>
#include <QImage>
#include <QSize>
#include <QRgb>
#include <QPainter>

MouseAndOrMotionViewMaker::MouseAndOrMotionViewMaker(QObject *parent) :
    QObject(parent), m_Initialized(false), m_ViewWidth(800), m_ViewHeight(600), m_MousePixmapToDraw(":/mouseCursor.svg")
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
QRect MouseAndOrMotionViewMaker::makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint)
{
    //start with a normal rectangle around the point
    QRect ret(inputPoint.x()-(m_ViewWidth/2),
              inputPoint.y()-(m_ViewHeight/2),
              m_ViewWidth,
              m_ViewHeight);

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
void MouseAndOrMotionViewMaker::startMakingMouseAndOrMotionViews()
{
    if(!m_Initialized)
    {
        emit d("failed to initialize screen, or wrong image format from grabWindow");
        return;
    }
    m_IntervalTimer.start(256);
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
        const QRect &rectWithinResolution = makeRectAroundPointStayingWithinResolution(currentCursorPosition);
#if 0
        int grabX = qMax(currentCursorPosition.x()-(m_ViewWidth/2), 0);
        int grabY = qMax(currentCursorPosition.y()-(m_ViewHeight/2), 0);

        //don't grab to the right of, or below, the screen
        int grabWidth = qMin((grabX+m_ViewWidth), m_ScreenResolutionX);
        int grabHeight = qMin((grabY+m_ViewHeight), m_ScreenResolutionY);
#endif
        //TODOqt4: QPixmap::grabWindow(), not sure if 'grabbing only primary screen' will work (and heck, surprised it's even working now in qt5)
        m_CurrentPixmap = m_Screen->grabWindow(0, //0 makes entire desktop eligible, restrained by a rect in following params
                                               rectWithinResolution.x(),
                                               rectWithinResolution.y(),
                                               m_ViewWidth, //my makeRect need only return topLeft point, fuck it
                                               m_ViewHeight);
        //draw mouse cursor
        {
            QPainter painter(&m_CurrentPixmap);
            //fucked, except in top-left case: painter.drawPixmap(currentCursorPosition.x(), currentCursorPosition.y(), m_MousePixmapToDraw.width(), m_MousePixmapToDraw.height(), m_MousePixmapToDraw);
            painter.drawPixmap(currentCursorPosition.x(), currentCursorPosition.y(), m_MousePixmapToDraw.width(), m_MousePixmapToDraw.height(), m_MousePixmapToDraw);
        }
        emit presentPixmapForViewingRequested(m_CurrentPixmap);
        m_PreviousPixmap = QPixmap(); //motion detection requires two non-mouse-movement frames in a row
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
            for(int j = 0; j < currentImageHeight; ++j) //TODOoptional: chop out the bottom right hand corner of screen from motion analysis, since my clock is updated every one second and i want to keep it that way...
            {
                const QRgb *currentImagePixel = (const QRgb*)(currentImage.constScanLine(j));
                const QRgb *previousImagePixel = (const QRgb*)(previousImage.constScanLine(j));
                for(int i = 0; i < currentImageWidth; ++i)
                {
                    if(*currentImagePixel != *previousImagePixel)
                    {
                        firstDifferenceSeenPoint = QPoint(i, j);
                        differenceSeen = true;
                        break;
                    }
                    ++currentImagePixel;
                    ++previousImagePixel;
                }
                if(differenceSeen)
                    break;
            }

            if(differenceSeen)
            {
#if MOUSE_DOESNT_MOVE_DURING_WINDOW_MOVE
                //then left -> right
                //the goal is to create a rectangle containing any motion

                differenceSeen = false; //each direction sets this back to false
                for(int i = 0; i < currentImageWidth; ++i)
                {
                    for(int j = 0; j < currentImageHeight; ++j)
                    {
                        const QRgb currentImagePixel = currentImage.pixel(i, j);
                        const QRgb previousImagePixel = previousImage.pixel(i, j);
                        if(currentImagePixel != previousImagePixel)
                        {
                            leftOfDiffRect = i;
                            differenceSeen = true;
                            break;
                        }
                    }
                    if(differenceSeen)
                        break;
                }

                //then right -> left

                differenceSeen = false;
                for(int i = currentImageWidth-1; i > -1; --i)
                {
                    for(int j = currentImageHeight-1; j > -1; --j)
                    {
                        const QRgb currentImagePixel = currentImage.pixel(i, j);
                        const QRgb previousImagePixel = previousImage.pixel(i, j);
                        if(currentImagePixel != previousImagePixel)
                        {
                            rightOfDiffRect = i;
                            differenceSeen = true;
                            break;
                        }
                    }
                    if(differenceSeen)
                        break;
                }

                //then bottom -> top

                differenceSeen = false;
                for(int j = currentImageHeight-1; j > -1; --j)
                {
                    for(int i = currentImageWidth-1; i > -1; --i)
                    {

                    }
                }
                //use "DiffRect"
#endif
                //use first difference seen point
                const QRect &rectWithinResolution = makeRectAroundPointStayingWithinResolution(firstDifferenceSeenPoint);
#if 0
                int motionX = qMax(firstDifferenceSeenPoint.x()-(m_ViewWidth/2), 0);
                int motionY = qMax(firstDifferenceSeenPoint.y()-(m_ViewHeight/2), 0);
                int motionWidth = qMin((motionX+m_ViewWidth), m_ScreenResolutionX);
                int motionHeight = qMin((motionY+m_ViewHeight), m_ScreenResolutionY);
#endif
                QPixmap motionPixmap = m_CurrentPixmap.copy(rectWithinResolution.x(), rectWithinResolution.y(), m_ViewWidth, m_ViewHeight); //sure we used QImage for pixel analysis, but we still have the QPixmap handy so woot saved a conversion
                emit presentPixmapForViewingRequested(motionPixmap);
            }
        }
    }
    m_PreviousPixmap = m_CurrentPixmap;
}
