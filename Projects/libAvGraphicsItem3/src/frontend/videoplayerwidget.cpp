#include "videoplayerwidget.h"
#ifndef QT_NO_OPENGL
    #include <QtOpenGL/QGLWidget>
#endif

videoPlayerWidget::videoPlayerWidget(QWidget *parent) :
    QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_VideoGraphicsScene = new QGraphicsScene(this);
    m_VideoGraphicsView = new QGraphicsView(m_VideoGraphicsScene);
    m_VideoGraphicsItem = new videoGraphicsItem();
#ifndef QT_NO_OPENGL
    m_VideoGraphicsView->setViewport(new QGLWidget);
#endif
    m_VideoGraphicsScene->addItem(m_VideoGraphicsItem);
    m_Layout->addWidget(m_VideoGraphicsView);

    m_ControlsLayout = new QHBoxLayout();
    m_PlayButton = new QPushButton();
    m_PlayButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_PlayButton, SIGNAL(clicked()), m_LibAvPlayer, SLOT(play()));
    m_ControlsLayout->addWidget(m_PlayButton);
    m_Layout->addLayout(m_ControlsLayout);

    this->setLayout(m_Layout);
}
