#ifndef AUDIOVIDEOBUFFERSWIDGET_H
#define AUDIOVIDEOBUFFERSWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>

#include <audiovideoplayer.h>

class AudioVideoBuffersWidget : public QWidget
{
    Q_OBJECT
public:
    AudioVideoBuffersWidget(QWidget *parent = 0);
    ~AudioVideoBuffersWidget();
private:
    QVBoxLayout *m_Layout;
    QPushButton *m_PlayButton;
    QPushButton *m_DestroyButton;
    QPlainTextEdit *m_DebugOutput;

    AudioVideoPlayer *m_AudioVideoPlayer;
private slots:
    void init();
};

#endif // AUDIOVIDEOBUFFERSWIDGET_H
