#include "mouseormotionorperiodofinactivitydetector.h"

#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QCursor>
#include <QPixmap>
#include <QPainter>

//TODOreq: although it would certainly be an optimization to give the motionDetected listeners a copy of the screen grab (it's COW, after all), I don't think this class should be responsible for cutting/slicing/cropping/scaling/etc. That should be the listender's job
//^Thinking about it more, it kind of does make sense to put the "rectangular cutting" into a lib that all the apps could then bum off of. Does it belong in this class, or it's own class [that then uses this class]? As per C[++] ideology, I should only implement it in this class if it doesn't cost anything when I don't use it. A simple setRectAroundMouseOrMotionToCutOutAndEmit(QRect) would get the job done. mouseMovementDetected would then emit a second arg, a QImage, that could simply be null whenever that setRectAround[...] hasn't been.. err... set. The movementOnScreenDetected signal stays the same, but now the secong arg is the rect image instead of the full desktop grab. Giving them the cut out rect/image does kinda sort defeat the purpose of giving them the mouse/motion coords though... wtf would they need em for? Maybe, though, JUST to keep the code cleaner/simpler and for no other reason, the image "cutting" should be put into it's own class. That's actually a decent reason. MouseOrMotionRectGrabberWithPeriodOfInactivityDetection
MouseOrMotionOrPeriodOfInactivityDetector::MouseOrMotionOrPeriodOfInactivityDetector(QObject *parent)
    : QObject(parent)
    , m_Screen(QGuiApplication::primaryScreen())
    , m_CurrentMousePos(-1, -1)
{
    if(!m_Screen || (m_Screen->grabWindow(0).toImage().format() != QImage::Format_RGB32))
        qFatal("failed to grab screen to verify it's format during MouseOrMotionOrPeriodOfInactivityDetector construction");
    m_ScreenResolution = m_Screen->size();
}
bool MouseOrMotionOrPeriodOfInactivityDetector::mouseMovedSinceLastPoll()
{
    bool ret = false;
    QPoint maybeNewMousePos = QCursor::pos();
    if(maybeNewMousePos != m_CurrentMousePos)
    {
        ret = true;
        m_CurrentMousePos = maybeNewMousePos;
    }
    return ret;
}
//TODOmb: we could let the user decide/specify what kind of screen grab gets emitted. We could then emit the screen grab (either QPixmap or QImage) in a QVariant. This really falls under the category of optimization since they are both losslessly interchangeable with each other
bool MouseOrMotionOrPeriodOfInactivityDetector::thereWasMotionOnScreenSinceLastPoll()
{
    QPixmap newScreenGrabPixmap = m_Screen->grabWindow(0, 0, 0, m_ScreenResolution.width(), m_ScreenResolution.height());
    QImage newScreenGrab = newScreenGrabPixmap.toImage();
    //if(m_OldScreenGrab != newScreenGrab) //TODOreq: a user specifiable list of 'shapes' (ignoring color contents) and/or 'pictures' (using color contents) to _EXCLUDE_ from triggering motion detection. One such example is the blinking text cursor in QtCreator. it might be faster to use the "!=" operator whenever both of those lists are empty, but idk might be the exact same lel. nvm disregard that, I can't use the "!=" operator anyways because I need to DETERMINE the point of motion
    if(!newScreenGrabMatchesOldScreenGrab_andBtwNoteThePointWhereTheyDontMatchPlox(newScreenGrab))
    {
        m_OldScreenGrab = newScreenGrab;
        return true;
    }
    return false;
}
bool MouseOrMotionOrPeriodOfInactivityDetector::thereHasBeenAperiodOfInactivity() const
{
    return false; //TODOreq
}
bool MouseOrMotionOrPeriodOfInactivityDetector::newScreenGrabMatchesOldScreenGrab_andBtwNoteThePointWhereTheyDontMatchPlox(const QImage &newScreenGrab)
{
    if(m_OldScreenGrab.isNull())
    {
        //corner case, first time in this method, let's just say they match, but let's take the new screen grab as our old screen grab so we don't get here again
        m_OldScreenGrab = newScreenGrab;
        return true;
    }
    if(newScreenGrab.size() != m_OldScreenGrab.size())
    {
        //don't crash/etc on resolution changes, just do nothing for now by saying they match, but we take the new screen grab as our old screen grab so we don't get here again
        m_OldScreenGrab = newScreenGrab;
        return true;
    }

    QImage mutableNewScreenGrab = newScreenGrab;

    //scan left to right bottom to top
    static const int m_BottomPixelRowsToIgnore = 16; //TODOreq: user specifiable, default to zero. a better way would be a list of "exclude regions", or conversely: "include regions", or both ;oP
    for(int y = ((mutableNewScreenGrab.height()-1)-m_BottomPixelRowsToIgnore); y > -1; --y)
    {
        QRgb *currentPixelOfNewScreenGrab = (QRgb*)(mutableNewScreenGrab.scanLine(y));
        QRgb *currentPixelOfOldScreenGrab = (QRgb*)(m_OldScreenGrab.scanLine(y));
        for(int x = 0; x < mutableNewScreenGrab.width(); ++x)
        {
            if(*currentPixelOfNewScreenGrab != *currentPixelOfOldScreenGrab)
            {
                //draw cursor on top of this first motion (make sure there is space above and to the right of it, don't go out of bounds (if it would have been out of bounds, the motion is valid)).... then continue onto finding motion. when a second motion is detected, we know it is NOT the cursor. if no more motion is seen, it was only the cursor
                if(!thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(y, x, mutableNewScreenGrab.width()))
                {
                    //the cursor would have gone out of bounds, so this motion is NOT the cursor
                    m_PointOnScreenWhereMotionWasDetected.setX(x);
                    m_PointOnScreenWhereMotionWasDetected.setY(y);
                    return false;
                }

                //TODOreq: my 'draw cursor over both images and continue' technique to detect the blinking cursor is shit. Say, for example, you type in a colon (:). If you draw a blinking cursor over the colon on both images like this, you will have erronesously excluded that colon from being considered 'motion'. I should instead cut out a rectangle the size of the cursor from each image, then compare/ensure that they are _NOT_ matching at EVERY PIXEL THEREIN. If that's the case, only then do I exclude it from being considered motion. But yea when you do detect an excluded cursor, you should then exclude those pixels from being checked later on (because otherwise we'd do the check again, and this time it would fail because we'd start 1 pixel 'higher' (assuming a 1 pixel wide cursor (which it isn't, but for the sake of explaining let's say it is))) -- One way to exclude the pixel rects from future checks is to draw the blinking cursor in like we were doing before (we just can't do that as our means of CHECKING)
                //^We don't necessariy have to 'cut out' the rectangle around the blinking cursor, but in our minds we do ;-P
                //^^One of the two rects, either old or new, must match pixel for pixel (including color, unless it's a 'shape' exclusion), the blinking cursor image!!! TODOreq
                //^^^Transparency 'exclude as motion' 'pictures' is a bitch, but possible: just ignore the transparent pixels kek: ie, every non-transparent pixel must NOT match between new and old for every [non-transparent] pixel therein. Fuck transparency atm tho it hurts my brain.

                //scan the blinking cursor 'exclude' image left to right bottom to top, just like with the images (the pixel we are looking at would then be the bottom-left pixel if it is the blinking cursor)
                bool oldScreenGrabHasQtCreatorCursorOnit = true;
                bool newScreenGrabHasQtCreatorCursorOnit = true;
                for(int y2 = (m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height()-1); y2 > -1; --y2)
                {
                    QRgb *currentPixelOfBlinkingCursor = (QRgb*)(m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.scanLine(y2));
                    for(int x2 = 0; x2 < m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.width(); ++x2)
                    {
                        QRgb currentPixelOfMaybeBlinkingCursorNewScreenGrab = mutableNewScreenGrab.pixel(x+x2, y-y2);
                        QRgb currentPixelOfMaybeBlinkingCursorOldScreenGrab = m_OldScreenGrab.pixel(x+x2, y-y2);

                        //compare new to old, ensure they DON'T match (see huge comment above)
                        if(currentPixelOfMaybeBlinkingCursorNewScreenGrab == currentPixelOfMaybeBlinkingCursorOldScreenGrab)
                        {
                            //them matching at any given pixel within the qt creator cursor rect means that x,y is a point of motion (ie, not a blinking qt cursor to be excluded)
                            m_PointOnScreenWhereMotionWasDetected.setX(x);
                            m_PointOnScreenWhereMotionWasDetected.setY(y);
                            return false;
                        }

                        //compare new to cursor (unless we already determined it doesn't match)
                        if(newScreenGrabHasQtCreatorCursorOnit && currentPixelOfMaybeBlinkingCursorNewScreenGrab != *currentPixelOfBlinkingCursor)
                            newScreenGrabHasQtCreatorCursorOnit = false;

                        //compare old to cursor (unless we already determined it doesn't match)
                        if(oldScreenGrabHasQtCreatorCursorOnit && currentPixelOfMaybeBlinkingCursorOldScreenGrab != *currentPixelOfBlinkingCursor)
                            oldScreenGrabHasQtCreatorCursorOnit = false;

                        if((!newScreenGrabHasQtCreatorCursorOnit) && (!oldScreenGrabHasQtCreatorCursorOnit))
                        {
                            //if neither screen grabs have the full blinking cursor on them, then we know the original point (x,y) was motion
                            m_PointOnScreenWhereMotionWasDetected.setX(x);
                            m_PointOnScreenWhereMotionWasDetected.setY(y);
                            return false;
                        }

                        ++currentPixelOfBlinkingCursor;
                    }
                }
                //if we get here, then we have determined that the motion was the blinking qt creator cursor that we want to exclude. since it's not just the current pixel we're on (x,y), we need to make sure that the rest of the blinking cursor's pixels don't also trigger non-matches. one way to do that is to draw the qt blinking cursor on both images (or just the one without it, but eh I'm lazy TODOoptimization)
                //draw qt blinking cursor and keep checking for other motion
                QPainter qtCreatorBlinkingCursorCurrentImagePainter(&mutableNewScreenGrab);
                qtCreatorBlinkingCursorCurrentImagePainter.drawImage(x, (y-m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height())+1, m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks); //TODOoptional: the +1 accounts for an off by one, which may also be present in thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching
                QPainter qtCreatorBlinkingCursorPreviousImagePainter(&m_OldScreenGrab);
                qtCreatorBlinkingCursorPreviousImagePainter.drawImage(x, (y-m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.height())+1, m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks);
            }
            ++currentPixelOfNewScreenGrab;
            ++currentPixelOfOldScreenGrab;
        }
    }
    return true;
}
bool MouseOrMotionOrPeriodOfInactivityDetector::thereIsEnoughRoomToDrawQtCreatorBlinkingCursorInOrderToExcludeItFromFutureSearching(int verticalPositionOfMotionPoint, int horizontalPositionOfMotionPoint, int imageWidth) const
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
//TODOmb: mouse poll rate independent of motion poll rate? since polling for mouse movements is cheap af by comparison. it would also make sense then to let the client specify a 'min time before emitting X-detected signals', because most likely they're going to be doing a screen grab (expensive), and those are NOT cheap. but then again since mouse uses it's own signal anyways, mayb that 'min time...' param isn't necessary (but still could be helpful so idfk)
void MouseOrMotionOrPeriodOfInactivityDetector::startDetectingMouseOrMotionOrPeriodsOfInactivity(int pollRateMSec, int amountOfTimeMSecWithNoMouseOrMotionActivityToBeConsideredAPeriodOfInactivity)
{
    m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks = QImage(2, 13, QImage::Format_RGB32); //TODOreq: use a better variable that let's me filter out N shapes/pictures, not just one
    m_QtCreatorBlinkingCursorToExcludeFromMotionDetectionChecks.fill(Qt::black);

    QTimer *pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(handlePollTimerTimedOut()));
    pollTimer->start(pollRateMSec);
}
void MouseOrMotionOrPeriodOfInactivityDetector::handlePollTimerTimedOut()
{
    if(mouseMovedSinceLastPoll())
        emit mouseMovementDetected(m_CurrentMousePos);
    else if(thereWasMotionOnScreenSinceLastPoll())
        emit motionOnScreenDetected(m_PointOnScreenWhereMotionWasDetected, m_OldScreenGrab /*new already became old*/);
    else if(thereHasBeenAperiodOfInactivity())
        emit periodOfInactivityDetected(); //TODOreq: only emit "once", not every poll thereafter
}
