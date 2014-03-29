#ifndef MOUSEANDORMOTIONVIEWMAKER_H
#define MOUSEANDORMOTIONVIEWMAKER_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QCursor>

class QScreen;

class MouseAndOrMotionViewMaker : public QObject
{
    Q_OBJECT
public:
    explicit MouseAndOrMotionViewMaker(QObject *parent = 0);
private:
    bool m_Initialized;
    QTimer m_IntervalTimer;
    QPoint m_PreviousCursorPosition;
    QScreen *m_Screen;
    int m_ScreenResolutionX;
    int m_ScreenResolutionY;
    int m_ViewWidth;
    int m_ViewHeight;
    QPixmap m_PreviousPixmap;
    QPixmap m_CurrentPixmap;
    QPixmap m_MousePixmapToDraw;

    QRect makeRectAroundPointStayingWithinResolution(const QPoint &inputPoint);
signals:
    void d(const QString &);
    void presentPixmapForViewingRequested(const QPixmap &);
public slots:
    void startMakingMouseAndOrMotionViews();
    void intervalTimerTimedOut();
};

#endif // MOUSEANDORMOTIONVIEWMAKER_H
