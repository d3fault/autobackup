#include "audiovideobufferswidget.h"

AudioVideoBuffersWidget::AudioVideoBuffersWidget(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}
void AudioVideoBuffersWidget::init()
{
    m_Layout = new QVBoxLayout();
    m_InflateButton = new QPushButton(tr("Start"));
    m_DeflateButton = new QPushButton(tr("Destroy"));
    m_DebugOutput = new QPlainTextEdit();

    m_Layout->addWidget(m_InflateButton);
    m_Layout->addWidget(m_DeflateButton);
    m_Layout->addWidget(m_DebugOutput);
    this->setLayout(m_Layout);

    connect(m_InflateButton, SIGNAL(clicked()), m_AudioVideoPlayer, SLOT(initAndProbe()));
    connect(m_DeflateButton, SIGNAL(clicked()), m_AudioVideoPlayer, SLOT(destroy()));
}
AudioVideoBuffersWidget::~AudioVideoBuffersWidget()
{

}
