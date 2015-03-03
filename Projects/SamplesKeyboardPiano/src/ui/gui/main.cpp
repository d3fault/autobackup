#include <QApplication>

#include "sampleskeyboardpianogui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SamplesKeyboardPianoGui gui;
    Q_UNUSED(gui)

    return a.exec();
}
