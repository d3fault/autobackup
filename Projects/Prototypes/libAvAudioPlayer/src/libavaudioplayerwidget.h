#ifndef LIBAVAUDIOPLAYERWIDGET_H
#define LIBAVAUDIOPLAYERWIDGET_H

#include <QtGui/QWidget>
#include <QThread>

class libAvAudioPlayer;

class libAvAudioPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    libAvAudioPlayerWidget(QWidget *parent = 0);
    ~libAvAudioPlayerWidget();
private:
    QThread *m_Thread;
    libAvAudioPlayer *m_Player;
private slots:
    void init();
};

#endif // LIBAVAUDIOPLAYERWIDGET_H
