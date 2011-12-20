#ifndef LIBAVPLAYERWIDGET_H
#define LIBAVPLAYERWIDGET_H

#include <QtGui/QWidget>
#include <QThread>
#include <QtMultimedia/QVideoFrame>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QStyle>

class LibAvPlayerBackend;
class LibAvGraphicsItem;
class RawAudioPlayer;

class libAvPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    libAvPlayerWidget(QWidget *parent = 0);
    ~libAvPlayerWidget();
private:
    void initGui();
    void initBackend();

    QThread *m_BackendThread;
    LibAvPlayerBackend *m_PlayerBackend;

    //gui
    QVBoxLayout         *m_Layout;
    QGraphicsView       *m_GraphicsView;
    QGraphicsScene      *m_GraphicsScene;
    LibAvGraphicsItem   *m_LibAvGraphicsItem;

    QHBoxLayout         *m_ControlsLayout;
    QPushButton         *m_PlayButton;
    QPushButton         *m_StopButton;

    //audio
    RawAudioPlayer *m_RawAudioPlayer;
private slots:
    void init();
};

#endif // LIBAVPLAYERWIDGET_H
