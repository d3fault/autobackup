#ifndef LIBAVAUDIOPLAYER2TEST_H
#define LIBAVAUDIOPLAYER2TEST_H

#include <QtGui/QWidget>
#include <QThread>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

#include <libavaudioplayer2.h>

class libAvAudioPlayer2test : public QWidget
{
    Q_OBJECT
public:
    libAvAudioPlayer2test(QWidget *parent = 0);
    ~libAvAudioPlayer2test();
private:
    QThread *m_Thread;
    libAvAudioPlayer2 *m_Player;

    //gui
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_PlayButton;
private slots:
    void init();
    void handleD(const QString &);
};

#endif // LIBAVAUDIOPLAYER2TEST_H
