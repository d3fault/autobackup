#include <QApplication>

#include <QDebug>
#include <QThread>

#include "libstk_loopaudiofileandmodifyitspitchusingasliderwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "main thread id:" << QThread::currentThreadId();

    LibStk_LoopAudioFileAndModifyItsPitchUsingAsliderWidget w;
    w.resize(100, 300);
    w.show();

    return a.exec();
}
