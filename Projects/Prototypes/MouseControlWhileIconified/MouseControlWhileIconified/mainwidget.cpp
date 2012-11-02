#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent), m_TimerStep(0)
{
    m_Layout = new QVBoxLayout();
    m_GoBtn = new QPushButton("Go");
    m_Layout->addWidget(m_GoBtn);

    setLayout(m_Layout);

    connect(m_GoBtn, SIGNAL(clicked()), this, SLOT(handleGo()));


    QAction *quit = new QAction("Quit", this);
    connect(quit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));

    m_TrayMenu = new QMenu(this);
    m_TrayMenu->addAction(quit);

    m_Tray = new QSystemTrayIcon(this);
    m_Tray->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxCritical));
    m_Tray->setContextMenu(m_TrayMenu);

    m_Tray->show();

    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(handleTimerTimedOut()));


    m_ScreenRect = m_DesktopWidget.availableGeometry();
}
mainWidget::~mainWidget()
{
    
}
void mainWidget::handleGo()
{
    this->hide();
    m_Timer->start(2000);
}
void mainWidget::handleTimerTimedOut()
{
    if(m_TimerStep < 5)
    {
        QPoint newPoint((qrand() % m_ScreenRect.width()), (qrand() % m_ScreenRect.height()));
        QCursor::setPos(newPoint);

#if FIGURE_OUT_MOUSE_CLICKS_N_SHIT_YO
        if(m_TimerStep == 3)
        {
            QTest::mouseClick(m_DesktopWidget.screen(), Qt::RightButton, 0, newPoint);

            //QMouseEvent *mousePressEvent = new QMouseEvent(QMouseEvent::MouseButtonPress, newPoint, Qt::RightButton, 0, 0);
            //QCoreApplication::sendEvent(m_DesktopWidget.screen(), &mousePressEvent);
            //QCoreApplication::sendEvent(this, &mousePressEvent);
            //QCoreApplication::postEvent(m_DesktopWidget.screen(), mousePressEvent);

            //QMouseEvent *mouseReleaseEvent = new QMouseEvent(QMouseEvent::MouseButtonRelease, newPoint, Qt::RightButton, 0, 0);
            //QCoreApplication::sendEvent(m_DesktopWidget.screen(), &mouseReleaseEvent);
            //QCoreApplication::postEvent(m_DesktopWidget.screen(), mouseReleaseEvent);
        }
#endif
    }
    else
    {
        m_Timer->stop();
        m_TimerStep = 0;
        this->show();
    }
    ++m_TimerStep;
}
