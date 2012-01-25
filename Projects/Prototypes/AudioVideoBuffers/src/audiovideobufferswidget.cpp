#include "audiovideobufferswidget.h"

AudioVideoBuffersWidget::AudioVideoBuffersWidget(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}
void AudioVideoBuffersWidget::init()
{
    m_Layout = new QVBoxLayout();
    m_PlayButton = new QPushButton(tr("Play"));
    m_DestroyButton = new QPushButton(tr("Destroy"));
    m_DebugOutput = new QPlainTextEdit();

    m_Layout->addWidget(m_PlayButton);
    m_Layout->addWidget(m_DestroyButton);
    m_Layout->addWidget(m_DebugOutput);
    this->setLayout(m_Layout);

    connect(m_PlayButton, SIGNAL(clicked()), m_AudioVideoPlayer, SLOT(initAndProbe()));
    connect(m_DestroyButton, SIGNAL(clicked()), m_AudioVideoPlayer, SLOT(destroy()));
}
AudioVideoBuffersWidget::~AudioVideoBuffersWidget()
{

}
