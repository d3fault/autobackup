#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QCursor>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QRect>
#include <QIcon>
#include <QStyle>
#include <QMouseEvent>

class mainWidget : public QWidget
{
    Q_OBJECT
public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
private:
    QDesktopWidget m_DesktopWidget;
    QPushButton *m_GoBtn;
    QTimer *m_Timer;
    QVBoxLayout *m_Layout;

    QRect m_ScreenRect;

    QMenu *m_TrayMenu;
    QSystemTrayIcon *m_Tray;

    quint8 m_TimerStep;
private slots:
    void handleGo();
    void handleTimerTimedOut();
};

#endif // MAINWIDGET_H
