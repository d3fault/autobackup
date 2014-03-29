#include "mouseandormotionviewmaker.h"

#include <QScreen>
#include <QCursor>
#include <QGuiApplication>
#include <QImage>
#include <QSize>
#include <QRgb>

#include <iostream>

MouseAndOrMotionViewMaker::MouseAndOrMotionViewMaker(QObject *parent) :
    QObject(parent), m_Initialized(false), m_ViewWidth(800), m_ViewHeight(600)
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
        int grabX = qMax(currentCursorPosition.x()-(m_ViewWidth/2), 0);
        int grabY = qMax(currentCursorPosition.y()-(m_ViewHeight/2), 0);

        //don't grab to the right of, or below, the screen
        int grabWidth = qMin((grabX+m_ViewWidth), m_ScreenResolutionX);
        int grabHeight = qMin((grabY+m_ViewHeight), m_ScreenResolutionY);
        m_CurrentPixmap = m_Screen->grabWindow(0, //0 makes entire desktop eligible, restrained by a rect in following params
                                               grabX,
                                               grabY,
                                               grabWidth,
                                               grabHeight);
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
            for(int j = 0; j < currentImageHeight; ++j)
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
                //code jacked from mouse movement
                int motionX = qMax(firstDifferenceSeenPoint.x()-(m_ViewWidth/2), 0);
                int motionY = qMax(firstDifferenceSeenPoint.y()-(m_ViewHeight/2), 0);
                int motionWidth = qMin((motionX+m_ViewWidth), m_ScreenResolutionX);
                int motionHeight = qMin((motionY+m_ViewHeight), m_ScreenResolutionY);
                QPixmap motionPixmap = m_CurrentPixmap.copy(motionX, motionY, motionWidth, motionHeight); //sure we used QImage for pixel analysis, but we still have the QPixmap handy so woot saved a conversion
                emit presentPixmapForViewingRequested(motionPixmap);
            }
        }
    }
    m_PreviousPixmap = m_CurrentPixmap;
}
