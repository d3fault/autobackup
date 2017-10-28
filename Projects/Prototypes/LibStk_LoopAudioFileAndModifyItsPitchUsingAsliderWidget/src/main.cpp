#include <QApplication>

#include "libstk_loopaudiofileandmodifyitspitchusingaslider.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LibStk_LoopAudioFileAndModifyItsPitchUsingAslider stkAndSlider;
    Q_UNUSED(stkAndSlider)

    return a.exec();
}
