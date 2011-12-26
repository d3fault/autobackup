#include <QtGui/QApplication>
#include "libavaudioplayer2test.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    libAvAudioPlayer2test w;
    w.show();

    return a.exec();
}
