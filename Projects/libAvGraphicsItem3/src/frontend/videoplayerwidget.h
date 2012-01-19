#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

#include "../backend/libavplayer.h"
#include "videographicsitem.h"

class videoPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit videoPlayerWidget(QWidget *parent = 0);
private:
    //IAVideoPlayer *m_VideoPlayer; //TODOreq: libAvPlayer : IVideoPlayer
    //TODO:^^^ can't be fucked right now, just want it to work
    libAvPlayer *m_LibAvPlayer;

    QVBoxLayout *m_Layout;
    QHBoxLayout *m_ControlsLayout;

    QGraphicsView *m_VideoGraphicsView;
    videoGraphicsItem *m_VideoGraphicsItem;
    QGraphicsScene *m_VideoGraphicsScene;

    QPushButton *m_PlayButton;
signals:

public slots:

};

#endif // VIDEOPLAYERWIDGET_H
